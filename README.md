# Real_time_weather_plot
Project hiển thị biểu đồ nhiệt độ, độ ẩm theo thời gian thực, được đọc thông qua cảm biến DHT11.
## Thành phần cứng:
- ESP32
- DHT11
- UART
## Phần mềm
- Arduino IDE
- Qt Creator
## Code phần cứng
- Sử dụng cảm biến DHT11 để đọc thông qua ESP32
- Giá trị dữ liệu được đến app thông qua kết nối UART
- Source code: [weather](https://github.com/ngocdaitran2003/Real_time_weather_plot/blob/main/weather.ino)
## Code phần mềm
- Sử dụng thư viện QSerialPort để nhận dữ liệu từ kết nối UART
- Sử dụng Qcustomplot để biểu diễn biểu đồ dữ liệu theo thời gian thực 
- Source code: [App](https://github.com/ngocdaitran2003/Real_time_weather_plot/tree/main/Qt_App)  
- Hình ảnh giao diện:
<img src="https://github.com/ngocdaitran2003/Real_time_weather_plot/blob/main/weather_plot.png">