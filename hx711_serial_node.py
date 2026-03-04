#!/usr/bin/env python3
"""
ROS2 Serial Node for HX711 Load Cell (Arduino)
Reads weight data from Arduino via Serial and publishes with ROS timestamp.

Published topic : load_cell/weight  (geometry_msgs/PointStamped)
  header.stamp    = ROS 수신 시각
  header.frame_id = 'load_cell'
  point.x         = weight [g]
  point.y / z     = 0.0 (미사용)

Arduino protocol:  "<float>\n"  예) "123.456\n"  @ 115200 baud
"""

import rclpy
from rclpy.node import Node
from std_msgs.msg import Header
from geometry_msgs.msg import PointStamped

import serial
import serial.tools.list_ports


class HX711SerialNode(Node):
    def __init__(self):
        super().__init__('hx711_serial_node')

        # ── Parameters ───────────────────────────────────────────────
        self.declare_parameter('port',          '/dev/ttyACM0')
        self.declare_parameter('baud_rate',     115200)
        self.declare_parameter('timeout',       1.0)
        self.declare_parameter('publish_topic', 'load_cell/weight')
        self.declare_parameter('frame_id',      'load_cell')

        self.port      = self.get_parameter('port').get_parameter_value().string_value
        self.baud_rate = self.get_parameter('baud_rate').get_parameter_value().integer_value
        self.timeout   = self.get_parameter('timeout').get_parameter_value().double_value
        self.frame_id  = self.get_parameter('frame_id').get_parameter_value().string_value
        topic          = self.get_parameter('publish_topic').get_parameter_value().string_value

        # ── Publisher ─────────────────────────────────────────────────
        self.pub_weight = self.create_publisher(PointStamped, topic, 10)

        # ── Serial ───────────────────────────────────────────────────
        self.serial_conn = None
        self._connect_serial()

        # ── Timer: poll at 100 Hz ────────────────────────────────────
        self.timer = self.create_timer(0.01, self.read_serial)

        self.get_logger().info(
            f"HX711 Serial Node started | port={self.port} | baud={self.baud_rate} | "
            f"topic={topic} | frame_id={self.frame_id}"
        )

    # ── Serial helpers ────────────────────────────────────────────────
    def _connect_serial(self):
        try:
            self.serial_conn = serial.Serial(
                port=self.port,
                baudrate=self.baud_rate,
                timeout=self.timeout
            )
            self.serial_conn.reset_input_buffer()
            self.get_logger().info(f"Serial connected: {self.port}")
        except serial.SerialException as e:
            self.get_logger().error(f"Failed to open serial port '{self.port}': {e}")
            self._list_available_ports()
            self.serial_conn = None

    def _list_available_ports(self):
        ports = serial.tools.list_ports.comports()
        if ports:
            self.get_logger().info("Available serial ports:")
            for p in ports:
                self.get_logger().info(f"  {p.device} - {p.description}")
        else:
            self.get_logger().warn("No serial ports found.")

    # ── Main callback ─────────────────────────────────────────────────
    def read_serial(self):
        if self.serial_conn is None or not self.serial_conn.is_open:
            return

        line = ''
        try:
            if self.serial_conn.in_waiting == 0:
                return

            # ① 수신 즉시 타임스탬프 기록 (지연 최소화)
            stamp = self.get_clock().now().to_msg()

            line = self.serial_conn.readline().decode('utf-8').strip()
            if not line:
                return

            weight = float(line)

            # ② PointStamped 메시지 구성
            msg = PointStamped()
            msg.header = Header()
            msg.header.stamp    = stamp
            msg.header.frame_id = self.frame_id
            msg.point.x = 0.0
            msg.point.y = 0.0
            msg.point.z = float(weight)

            self.pub_weight.publish(msg)

            self.get_logger().debug(
                f"[{stamp.sec}.{stamp.nanosec:09d}] Weight: {weight:.3f} g"
            )

        except ValueError:
            self.get_logger().warn(f"Parse error (skipping): '{line}'")
        except serial.SerialException as e:
            self.get_logger().error(f"Serial error: {e}")
            self.serial_conn = None
        except UnicodeDecodeError as e:
            self.get_logger().warn(f"Decode error (skipping): {e}")

    # ── Cleanup ───────────────────────────────────────────────────────
    def destroy_node(self):
        if self.serial_conn and self.serial_conn.is_open:
            self.serial_conn.close()
            self.get_logger().info("Serial port closed.")
        super().destroy_node()


# ======================================================================
def main(args=None):
    rclpy.init(args=args)
    node = HX711SerialNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
