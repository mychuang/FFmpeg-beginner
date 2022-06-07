# FFMPEG 技術評估報告
refer: https://github.com/leandromoreira/ffmpeg-libav-tutorial#transmuxing

## Intro

- CODEC: 編解碼器

  透過編解碼器可以壓縮(解壓縮)視訊流<p>
  試計算以下範例的原始影像大小: <p>
  
  ```python
  toppf = 1080 * 1920 //total_of_pixels_per_frame
  cpp = 3 //cost_per_pixel
  tis = 30 * 60 //time_in_seconds
  fps = 24 //frames_per_second

  required_storage = tis * fps * toppf * cpp #250.28GB
  ```
  30分鐘1080*1920解析度, 24fps的彩色影像(3通道)，就需要250.28G容量，That's why we need to use a CODEC.

- container: 容器(封裝格式)

  用於打包試頻或音頻的容器，又叫封裝格式，一般可透過文件副檔名來判斷容器格式，例如:<p>

  |容器格式 | 視訊編碼格式| 音訊編瑪格式|
  |------- |------------| -----------|
  | MPEG4  |    h264   |     AAC    |    
  | Webm   |    vp9   |     opus    |

  還有其他的容器，例如: AVI, MOV, WMV...等

## FFmpeg - command line

- 安裝ffmpeg

  refer: https://www.geeksforgeeks.org/how-to-install-ffmpeg-on-windows/

- 切換封裝格式

  ```shell
  ffmpeg -i input.mp4 output.avi
  ```


## Learn FFmpeg libav the Hard Way