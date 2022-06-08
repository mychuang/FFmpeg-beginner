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

## FFmpeg tools

- 安裝ffmpeg

  refer: https://www.geeksforgeeks.org/how-to-install-ffmpeg-on-windows/

- ffmpeg：a command line tool to convert multimedia files between formats.

  ```shell
  ffmpeg -i input.mp4 output.avi
  ```

- ffplay：a simple media player based on SDL and the FFmpeg libraries.

  ```shell
  ffplay input.mp4
  ```

- ffprobe：a simple multimedia stream analyzer.

  ```shell
  ffprobe input.mp4
  ```

## FFmpeg lib

- libavutil：包含simplifying programming, 隨機數產生器, 資料結構, 數學運算, 核心多媒體實用程序, 等等函式庫.

- libavcodec：音訊/視訊邊解碼器的函式庫.

- libavformat：包含多媒體容器格式的demuxers and muxers.

- libavdevice：包含輸入和輸出設備的函式庫，用於許多常見的多媒體輸入/輸出軟體框架中，例如 
Video4Linux, Video4Linux2, VfW, and ALSA.

- libavfilter：libavfilter is a library containing media filters.

- libswscale：用於執行高度優化的圖像縮放和 color space/pixel 格式轉換的函式庫.

- libswresample：用於執行高度優化 audio resampling, rematrixing 和 sample format conversion operations 的函式庫




