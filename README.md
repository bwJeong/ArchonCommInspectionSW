# Archon control driver
- Development tool: C++ / Qt5

## 어떠한 작업을 수행하는 S/W인가요?
- 총 3대의 CCD 컨트롤러를 병행 제어하기 위한 통신 인터페이스 점검 S/W
- 사용 CCD 컨트롤러: Archon (http://www.sta-inc.net/archon/)

## 커뮤니케이션
- 이더넷 소켓 통신 (Archon <-> PC)
- 총 3대의 Archon (Guide, Science 1, Science 2)은 Gigabit ethernet을 통해 PC와 Communication을 수행

## 
- Connection 메뉴


![Connection menu](https://user-images.githubusercontent.com/54430715/98506508-02f9c580-229f-11eb-9e8c-07db777ca2ca.PNG)


| # | Control | Description |
|:-:|:-------:|-------------|
|1|Line edit|이더넷 소켓 통신 연결을 수행할 IP 주소를 입력하는 곳.<br/>- Guide: 10.0.0.2 (Default)<br/>- Sci_1: 10.0.0.12 (Default)<br/>- Sci_2: 10.0.0.13 (Default)|
|2|Line edit|이더넷 소켓 통신 연결을 수행할 Port number를 입력하는 곳.<br/>- Guide: 4242 (Default)<br/>- Sci_1: 4242 (Default)<br/>- Sci_2: 4242 (Default)|
|3|Toggle button|1, 2에 입력된 IP 주소 및 Port number를 기반으로 이더넷 소켓 통신 연결 및 해제를 수행.|
<br/>

- Guide 메뉴


![Guide menu](https://user-images.githubusercontent.com/54430715/98506511-05f4b600-229f-11eb-9065-217f7d0fe3fe.PNG)


| # | Control | Description |
|:-:|:-------:|-------------|
|1|List widget|PC에서 Archon 컨트롤러로 송신되는 커맨드 로그가 기록되는 위젯|
|2|Toggle button|1에서 기록되는 커맨드 로그를 텍스트 형식의 파일로 저장하는 기능. 저장된 파일은 본 응용프로그램의 실행파일 위치에 자동으로 생성됨.<br/>- 파일명 형식: “Guide_yyMMddThhmmss_TxLog.txt”|
|3|List widget|커맨드 송신에 따라 Archon 컨트롤러에서 PC로 수신되는 Ack 로그가 기록되는 위젯.|
|4|Toggle button|3에서 기록되는 Ack 로그를 텍스트 형식의 파일로 저장하는 기능. 저장된 파일은 본 응용프로그램의 실행파일 위치에 자동으로 생성됨.<br/>- 파일명 형식: “Guide_yyMMddThhmmss_RxLog.txt”|
|5|Line edit|6을 통해 불러온 config file (&#42;.acf)의 경로가 입력되는 곳.|
|6|Button|Archon 컨트롤러의 Configuration 설정을 위한 config file을 불러옴.|
|7|Button|6을 통해 불러온 config file을 Archon 컨트롤러에 적용하는 커맨드를 전송.|
|8|Button|Archon 컨트롤러에 CCD 전원을 키는 커맨드를 전송.|
|9|Button|Archon 컨트롤러에 CCD 전원을 끄는 커맨드를 전송.|
|10|Button|Archon 컨트롤러에 영상 촬영(Integration time: 1000 ms)을 수행하는 커맨드를 전송. 이후 Archon 컨트롤러에 프레임 상태를 확인하는 커맨드를 전송함으로써, 새로운 프레임이 갱신되었는지를 확인.|
|11|Button|Archon 컨트롤러에 촬영 영상을 PC로 전달하는 커맨드를 전송. 이후 PC로 전달된 영상에 Archon 컨트롤러의 상태 정보 및 촬영 Date/time을 포함한 FITS 헤더를 추가하여, 본 응용프로그램의 실행파일 위치에 FITS file (&#42;.fits)을 생성함.<br/>- 파일명 형식: “Guide&#95;[width]x[height]&#95;1000ms&#95;[frame&#95;number].fits”|
<br/>

- Sci_1 / Sci_2 메뉴


![Sci_1nSci_2 menu](https://user-images.githubusercontent.com/54430715/98506515-07be7980-229f-11eb-9260-653db33676d6.PNG)


| # | Control | Description |
|:-:|:-------:|-------------|
|1|List widget|PC에서 Archon 컨트롤러로 송신되는 커맨드 로그가 기록되는 위젯|
|2|Toggle button|1에서 기록되는 커맨드 로그를 텍스트 형식의 파일로 저장하는 기능. 저장된 파일은 본 응용프로그램의 실행파일 위치에 자동으로 생성됨.<br/>- 파일명 형식: “Sci1_yyMMddThhmmss_TxLog.txt”<br/>- 파일명 형식: “Sci2_yyMMddThhmmss_TxLog.txt”|
|3|List widget|커맨드 송신에 따라 Archon 컨트롤러에서 PC로 수신되는 Ack 로그가 기록되는 위젯.|
|4|Toggle button|3에서 기록되는 Ack 로그를 텍스트 형식의 파일로 저장하는 기능. 저장된 파일은 본 응용프로그램의 실행파일 위치에 자동으로 생성됨.<br/>- 파일명 형식: “Sci1_yyMMddThhmmss_RxLog.txt”<br/>- 파일명 형식: “Sci2_yyMMddThhmmss_RxLog.txt”|
|5|Line edit|6을 통해 불러온 config file (&#42;.acf)의 경로가 입력되는 곳.|
|6|Button|Archon 컨트롤러의 Configuration 설정을 위한 config file을 불러옴.|
|7|Button|6을 통해 불러온 config file을 Archon 컨트롤러에 적용하는 커맨드를 전송.|
|8|Button|Archon 컨트롤러에 CCD 전원을 키는 커맨드를 전송.|
|9|Button|Archon 컨트롤러에 CCD 전원을 끄는 커맨드를 전송.|
|10|Button|Archon 컨트롤러에 영상 촬영(Integration time: 1000 ms)을 수행하는 커맨드를 전송. 이후 Archon 컨트롤러에 프레임 상태를 확인하는 커맨드를 전송함으로써, 새로운 프레임이 갱신되었는지를 확인.|
|11|Button|Archon 컨트롤러에 촬영 영상을 PC로 전달하는 커맨드를 전송. 이후 PC로 전달된 영상에 Archon 컨트롤러의 상태 정보 및 촬영 Date/time을 포함한 FITS 헤더를 추가하여, 본 응용프로그램의 실행파일 위치에 FITS file (&#42;.fits)을 생성함.<br/>- 파일명 형식: “Sci1&#95;[width]x[height]&#95;1000ms&#95;[frame&#95;number].fits”<br/>- 파일명 형식: “Sci2&#95;[width]x[height]&#95;1000ms&#95;[frame&#95;number].fits”|

