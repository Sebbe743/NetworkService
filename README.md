# NetworkService

## introduction

This project contains a ZMQ service for the weather. There will be multiple options provided (details will follow). The idea is to have easier communications with the open-meteo api.

## Usage info

The service will be available on Benternet. By subscribing to it you can get weather data on specific coordinates or for the default coordinates. The default is set to the Corda campus in Hasselt. The subscribing to the server (by the client) goes as followed:

1: User connects to Benternet ip: ```tcp://benternet.pxl-ea-ict.be ``` (on both ```ports: 24041,24042```)

2: User subscribes (and receives): ```Sebbe>subbed>LON,LAT>OPTION``` (Lon, lat and option are variables. Lon and Lat are numbers with 2 decimals. option is an integer of 0-3)
   User sends to ```Sebbe>pushed>```

3: Data is returned

![image](https://user-images.githubusercontent.com/79916322/236616521-03c9af9a-e66f-4223-b867-d576354ae41f.png)



### Options

0: Default, all data gets sent

1: temperature

2: windspeed, winddirection

3: temperature, windspeed, winddirection



# TODO 

Adding geocoding api for extra service
