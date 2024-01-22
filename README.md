# Home automation web

## Introduction
This project is created as part of a bigger school project (you can find it [here](https://docs.google.com/document/d/1TYbxPwrkP0z3PoIvjMK5cm0hmrtCHO2-GA9gFeh20FY/edit?usp=sharing) (Rus.))
in which I build an air quality monitor using simple off-the-shelf components. 

This repository contains code for the microcontroller and the server. 
## Quick start
Clone the project
``` bash 
git clone https://github.com/yura34054/home-automation-web.git
cd home-automation-web
```

Create .env file, don't forget to change the settings
``` bash
cp .env.example .env
```

Now you can build the image and run everything in container with one command
``` bash
make
```

To upload code to the ESP32-c6 you can use esp-idf or arduino IDE

## Possible problems
* Right now server side does not support multiple sensors/monitors


## Getting help
If you have a problem setting up or a question you can contact me:

telegram: https://t.me/Yurii_Mironov

email: ur34054@gmail.com