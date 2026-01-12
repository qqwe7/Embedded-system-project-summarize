#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
简易逻辑分析仪 - PC端图形化上位机软件

功能：
1. 通过串口与 GD32F103RCT6 通信
2. 图形化界面设置采样参数
3. 实时显示采样波形
"""

import tkinter as tk
from tkinter import ttk, messagebox, scrolledtext
import serial
import serial.tools.list_ports
import threading
import time

class LogicAnalyzerGUI:
    """逻辑分析仪图形界面"""
    
    def __init__(self, root):
        self.root = root
        self.root.title("简易逻辑分析仪 v1.0 - GD32F103RCT6")
        self.root.geometry("900x700")
        self.root.minsize(800, 600)
        
        self.ser = None
        self.is_connected = False
        self.sample_data = []
        
        self.create_widgets()
        self.refresh_ports()
        
    def create_widgets(self):
        """创建界面控件"""
        
        # ====== 顶部连接区域 ======
        conn_frame = ttk.LabelFrame(self.root, text="串口连接", padding=10)
        conn_frame.pack(fill=tk.X, padx=10, pady=5)
        
        ttk.Label(conn_frame, text="串口:").pack(side=tk.LEFT)
        self.port_combo = ttk.Combobox(conn_frame, width=15, state="readonly")
        self.port_combo.pack(side=tk.LEFT, padx=5)
        
        ttk.Button(conn_frame, text="刷新", command=self.refresh_ports).pack(side=tk.LEFT)
        
        ttk.Label(conn_frame, text="波特率:").pack(side=tk.LEFT, padx=(20, 0))
        self.baud_combo = ttk.Combobox(conn_frame, width=10, state="readonly",
                                        values=["9600", "19200", "38400", "57600", "115200", "230400", "460800", "921600"])
        self.baud_combo.set("115200")
        self.baud_combo.pack(side=tk.LEFT, padx=5)
        
        self.conn_btn = ttk.Button(conn_frame, text="连接", command=self.toggle_connection)
        self.conn_btn.pack(side=tk.LEFT, padx=10)
        
        self.status_label = ttk.Label(conn_frame, text="● 未连接", foreground="red")
        self.status_label.pack(side=tk.LEFT, padx=10)
        
        # ====== 参数设置区域 ======
        param_frame = ttk.LabelFrame(self.root, text="采样参数", padding=10)
        param_frame.pack(fill=tk.X, padx=10, pady=5)
        
        # 采样率
        ttk.Label(param_frame, text="预分频(PSC):").grid(row=0, column=0, sticky=tk.W)
        self.psc_entry = ttk.Entry(param_frame, width=8)
        self.psc_entry.insert(0, "71")
        self.psc_entry.grid(row=0, column=1, padx=5)
        
        ttk.Label(param_frame, text="重装载(ARR):").grid(row=0, column=2, sticky=tk.W, padx=(20, 0))
        self.arr_entry = ttk.Entry(param_frame, width=8)
        self.arr_entry.insert(0, "9")
        self.arr_entry.grid(row=0, column=3, padx=5)
        
        self.rate_label = ttk.Label(param_frame, text="采样率: 100.0 kHz")
        self.rate_label.grid(row=0, column=4, padx=20)
        
        ttk.Button(param_frame, text="计算", command=self.calc_rate).grid(row=0, column=5)
        ttk.Button(param_frame, text="设置采样率", command=self.set_rate).grid(row=0, column=6, padx=10)
        
        # 采样数量
        ttk.Label(param_frame, text="采样数量:").grid(row=1, column=0, sticky=tk.W, pady=5)
        self.count_entry = ttk.Entry(param_frame, width=8)
        self.count_entry.insert(0, "1024")
        self.count_entry.grid(row=1, column=1, padx=5)
        ttk.Button(param_frame, text="设置数量", command=self.set_count).grid(row=1, column=2, padx=10)
        
        # 触发设置
        ttk.Label(param_frame, text="触发引脚:").grid(row=1, column=3, sticky=tk.W, padx=(20, 0))
        self.trig_pin_combo = ttk.Combobox(param_frame, width=6, state="readonly",
                                            values=["PA0", "PA1", "PA2", "PA3", "PA4", "PA5", "PA6", "PA7"])
        self.trig_pin_combo.set("PA0")
        self.trig_pin_combo.grid(row=1, column=4, padx=5)
        
        ttk.Label(param_frame, text="边沿:").grid(row=1, column=5)
        self.trig_edge_combo = ttk.Combobox(param_frame, width=8, state="readonly",
                                             values=["上升沿", "下降沿"])
        self.trig_edge_combo.set("上升沿")
        self.trig_edge_combo.grid(row=1, column=6, padx=5)
        
        ttk.Button(param_frame, text="设置触发", command=self.set_trigger).grid(row=1, column=7, padx=5)
        ttk.Button(param_frame, text="禁用触发", command=self.disable_trigger).grid(row=1, column=8, padx=5)
        
        # ====== 控制按钮区域 ======
        ctrl_frame = ttk.Frame(self.root, padding=10)
        ctrl_frame.pack(fill=tk.X, padx=10)
        
        self.cap_btn = ttk.Button(ctrl_frame, text="▶ 开始采样", command=self.start_capture)
        self.cap_btn.pack(side=tk.LEFT, padx=5)
        
        ttk.Button(ctrl_frame, text="📊 获取数据", command=self.get_data).pack(side=tk.LEFT, padx=5)
        ttk.Button(ctrl_frame, text="❓ 帮助", command=self.send_help).pack(side=tk.LEFT, padx=5)
        ttk.Button(ctrl_frame, text="🗑️ 清空日志", command=self.clear_log).pack(side=tk.LEFT, padx=5)
        
        # ====== 波形显示区域 ======
        wave_frame = ttk.LabelFrame(self.root, text="波形显示 (PA0-PA7)", padding=10)
        wave_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)
        
        # 波形画布
        self.canvas = tk.Canvas(wave_frame, bg="black", height=300)
        self.canvas.pack(fill=tk.BOTH, expand=True)
        
        # 滚动条
        scroll_frame = ttk.Frame(wave_frame)
        scroll_frame.pack(fill=tk.X)
        self.h_scroll = ttk.Scrollbar(scroll_frame, orient=tk.HORIZONTAL, command=self.scroll_wave)
        self.h_scroll.pack(fill=tk.X)
        
        self.wave_offset = 0
        self.wave_zoom = 1.0
        
        zoom_frame = ttk.Frame(scroll_frame)
        zoom_frame.pack()
        ttk.Button(zoom_frame, text="放大", command=lambda: self.zoom_wave(1.5)).pack(side=tk.LEFT, padx=5)
        ttk.Button(zoom_frame, text="缩小", command=lambda: self.zoom_wave(0.67)).pack(side=tk.LEFT, padx=5)
        ttk.Button(zoom_frame, text="重置", command=lambda: self.zoom_wave(0)).pack(side=tk.LEFT, padx=5)
        
        # ====== 日志区域 ======
        log_frame = ttk.LabelFrame(self.root, text="通信日志", padding=5)
        log_frame.pack(fill=tk.BOTH, padx=10, pady=5)
        
        self.log_text = scrolledtext.ScrolledText(log_frame, height=8, state=tk.DISABLED, font=("Consolas", 9))
        self.log_text.pack(fill=tk.BOTH, expand=True)
        
    def refresh_ports(self):
        """刷新串口列表"""
        ports = [p.device for p in serial.tools.list_ports.comports()]
        self.port_combo['values'] = ports
        if ports:
            self.port_combo.set(ports[0])
        self.log("刷新串口列表: " + str(ports))
        
    def toggle_connection(self):
        """连接/断开串口"""
        if self.is_connected:
            self.disconnect()
        else:
            self.connect()
            
    def connect(self):
        """连接串口"""
        port = self.port_combo.get()
        baud = int(self.baud_combo.get())
        
        if not port:
            messagebox.showerror("错误", "请选择串口")
            return
            
        try:
            self.ser = serial.Serial(port, baud, timeout=1)
            time.sleep(0.1)
            self.ser.reset_input_buffer()
            
            self.is_connected = True
            self.conn_btn.config(text="断开")
            self.status_label.config(text="● 已连接 " + port, foreground="green")
            self.log(f"已连接到 {port} @ {baud}bps")
            
        except Exception as e:
            messagebox.showerror("连接失败", str(e))
            self.log(f"连接失败: {e}")
            
    def disconnect(self):
        """断开串口"""
        if self.ser:
            self.ser.close()
            self.ser = None
            
        self.is_connected = False
        self.conn_btn.config(text="连接")
        self.status_label.config(text="● 未连接", foreground="red")
        self.log("已断开连接")
        
    def send_command(self, cmd):
        """发送命令"""
        if not self.is_connected:
            messagebox.showwarning("警告", "请先连接串口")
            return None
            
        try:
            self.ser.reset_input_buffer()
            self.ser.write((cmd + '\n').encode())
            self.log(f"发送: {cmd}")
            time.sleep(0.1)
            
            lines = []
            while True:
                line = self.ser.readline().decode('utf-8', errors='ignore').strip()
                if not line:
                    break
                lines.append(line)
                if line == 'END':
                    break
                    
            for line in lines:
                self.log(f"接收: {line}")
                
            return lines
            
        except Exception as e:
            self.log(f"通信错误: {e}")
            return None
            
    def calc_rate(self):
        """计算采样率"""
        try:
            psc = int(self.psc_entry.get())
            arr = int(self.arr_entry.get())
            rate = 72000000 / (psc + 1) / (arr + 1)
            
            if rate >= 1000000:
                self.rate_label.config(text=f"采样率: {rate/1000000:.2f} MHz")
            else:
                self.rate_label.config(text=f"采样率: {rate/1000:.2f} kHz")
        except:
            pass
            
    def set_rate(self):
        """设置采样率"""
        psc = self.psc_entry.get()
        arr = self.arr_entry.get()
        self.send_command(f"RATE {psc} {arr}")
        self.calc_rate()
        
    def set_count(self):
        """设置采样数量"""
        count = self.count_entry.get()
        self.send_command(f"COUNT {count}")
        
    def set_trigger(self):
        """设置触发"""
        pin = self.trig_pin_combo.current()
        edge = 1 if self.trig_edge_combo.get() == "上升沿" else 0
        self.send_command(f"TRIG {pin} {edge}")
        
    def disable_trigger(self):
        """禁用触发"""
        self.send_command("NOTRIG")
        
    def start_capture(self):
        """开始采样"""
        self.cap_btn.config(state=tk.DISABLED)
        self.log("开始采样...")
        
        def capture_thread():
            self.send_command("CAP")
            # 等待采样完成
            time.sleep(0.5)
            for _ in range(20):
                resp = self.send_command("STATUS")
                if resp and any('READY' in s for s in resp):
                    break
                time.sleep(0.2)
            
            # 获取数据
            self.get_data()
            self.root.after(0, lambda: self.cap_btn.config(state=tk.NORMAL))
            
        threading.Thread(target=capture_thread, daemon=True).start()
        
    def get_data(self):
        """获取采样数据"""
        resp = self.send_command("SEND")
        if resp:
            self.sample_data = self.parse_data(resp)
            self.log(f"收到 {len(self.sample_data)} 个采样点")
            self.draw_waveform()
            
    def parse_data(self, response):
        """解析十六进制数据"""
        data = []
        in_data = False
        
        for line in response:
            if line.startswith('DATA:'):
                in_data = True
                hex_str = line[5:]
                if hex_str:
                    data.extend(self._parse_hex_line(hex_str))
            elif line == 'END':
                break
            elif in_data:
                data.extend(self._parse_hex_line(line))
                
        return data
        
    def _parse_hex_line(self, line):
        """解析一行十六进制数据"""
        values = []
        for i in range(0, len(line), 2):
            try:
                byte_val = int(line[i:i+2], 16)
                values.append(byte_val)
            except ValueError:
                pass
        return values
        
    def draw_waveform(self):
        """绘制波形"""
        self.canvas.delete("all")
        
        if not self.sample_data:
            self.canvas.create_text(400, 150, text="无数据", fill="white", font=("Arial", 20))
            return
            
        width = self.canvas.winfo_width()
        height = self.canvas.winfo_height()
        
        if width < 10 or height < 10:
            return
            
        num_channels = 8
        ch_height = height / num_channels
        
        colors = ["#00FF00", "#FF0000", "#00FFFF", "#FFFF00", 
                  "#FF00FF", "#FFA500", "#FFFFFF", "#00FF80"]
        
        samples = len(self.sample_data)
        visible_samples = int(samples / self.wave_zoom)
        start_idx = int(self.wave_offset * (samples - visible_samples)) if samples > visible_samples else 0
        end_idx = min(start_idx + visible_samples, samples)
        
        if end_idx <= start_idx:
            return
            
        x_scale = width / (end_idx - start_idx)
        
        for ch in range(num_channels):
            y_base = ch * ch_height
            y_low = y_base + ch_height * 0.8
            y_high = y_base + ch_height * 0.2
            
            # 通道标签
            self.canvas.create_text(30, y_base + ch_height / 2, 
                                    text=f"PA{ch}", fill=colors[ch], font=("Arial", 9, "bold"))
            
            # 绘制波形
            prev_x = 40
            prev_y = y_low
            
            for i in range(start_idx, end_idx):
                x = 40 + (i - start_idx) * x_scale
                bit = (self.sample_data[i] >> ch) & 1
                y = y_high if bit else y_low
                
                if i > start_idx:
                    # 垂直线
                    if prev_y != y:
                        self.canvas.create_line(x, prev_y, x, y, fill=colors[ch])
                    # 水平线
                    self.canvas.create_line(prev_x, prev_y, x, prev_y, fill=colors[ch])
                    
                prev_x = x
                prev_y = y
                
            # 最后一段
            self.canvas.create_line(prev_x, prev_y, width - 10, prev_y, fill=colors[ch])
            
    def scroll_wave(self, *args):
        """滚动波形"""
        if args[0] == 'moveto':
            self.wave_offset = float(args[1])
        elif args[0] == 'scroll':
            self.wave_offset += float(args[1]) * 0.1
        self.wave_offset = max(0, min(1, self.wave_offset))
        self.draw_waveform()
        
    def zoom_wave(self, factor):
        """缩放波形"""
        if factor == 0:
            self.wave_zoom = 1.0
        else:
            self.wave_zoom *= factor
            self.wave_zoom = max(0.1, min(100, self.wave_zoom))
        self.draw_waveform()
        
    def send_help(self):
        """发送帮助命令"""
        self.send_command("HELP")
        
    def clear_log(self):
        """清空日志"""
        self.log_text.config(state=tk.NORMAL)
        self.log_text.delete(1.0, tk.END)
        self.log_text.config(state=tk.DISABLED)
        
    def log(self, msg):
        """添加日志"""
        self.log_text.config(state=tk.NORMAL)
        self.log_text.insert(tk.END, f"[{time.strftime('%H:%M:%S')}] {msg}\n")
        self.log_text.see(tk.END)
        self.log_text.config(state=tk.DISABLED)


def main():
    root = tk.Tk()
    app = LogicAnalyzerGUI(root)
    
    # 窗口关闭事件
    def on_closing():
        if app.is_connected:
            app.disconnect()
        root.destroy()
        
    root.protocol("WM_DELETE_WINDOW", on_closing)
    
    # 窗口大小改变时重绘波形
    def on_resize(event):
        if app.sample_data:
            app.draw_waveform()
    app.canvas.bind("<Configure>", on_resize)
    
    root.mainloop()


if __name__ == '__main__':
    main()
