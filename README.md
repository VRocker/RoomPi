# RoomPi
Room monitoring with a Raspberry Pi

# Cloning the repository
RoomPi makes use of Buildroot which is added a submodule so once you have cloned this repo you will need to initialise the submodules. This is done by running the following commands:

```
  git submodule init
  git submodule update
```

# Building
The first time you build RoomPi you will need to initialise Buildroot with our configuration files, this is done by running ```make setup```
This will copy across our makefiles and build Buildroot for the first time.

Once the first build has run, simply run ```make```. This will build the custom packages followed by buildroot. Everything will then be tied together an a zImage will be created in the root directory

# Installing
RoomPi is a self-contained RAM-based operating system which runs on the Raspberry Pi. To boot the OS you will need to format the SD card in a specific way:

* Delete any existing partitions on the SD card
* Create a new FAT32 partition at the beginning of the drive, it should be at least 128MB. This will be the boot partition.
* Copy the Raspberry Pi firmware files to the boot partition. These can be found in ```buildroot/output/images/rpi-firmware```
* Copy the DTB file to the boot partiton. This is located in ```buildroot/output/images/```
* Copy the zImage to your boot partition. This should be in your root directory

You will also need to create a secondary partition for the configuration files to be stored. For this i have chosen to use F2FS as it's a nice flash-friendly file system. This partition can be as big as you want but should be the second partition on the SD card.

# First boot
Once the SD card has been setup you can boot he Raspberry Pi. It should load to a login prompt to which the default credentials are:

```
User: root
Password: roompi
```

# RoomPi Config File
#####/pi/etc/roompi.conf
The RoomPi config file will be used to setup your device ready for the API usage. You will need to add your API key for this device (obtainable from http://vrocker-hosting.ovh/roompi/dashboard) to the config file before you do anything else.

```
API_KEY=<your key> # The API key for your device which is found from (http://vrocker-hosting.ovh/roompi/dashboard)
TEMP_INTERVAL=300 # This is the recommended interval for sending updates to the API
DHT_PIN=<pin> # This depends on what GPIO pin your temp sensor is connected to (typically GPIO25)
DHT_TYPE=<type> # This will be either; 0 = DHT11, 1 = DHT22
PI_TYPE=<pi_type> # The type of Pi that you are running; 0 = Pi Zero, 1 = Pi, 2 = Pi 2, 3 = Pi 3
```

You will need to use vi to modify your configuration file.

# WiFi Configuration

Once this is set, your last step is to configure the wifi (if applicable). This is simply a case of running the following command:
```
wpa_passphrase <SSID> <Passphrase> >> /etc/wpa_supplicant.conf
```

# Finishing things off

Your final step is to save the configuration. As the operating system runs entirely in RAM, no changes are persistant unless they are saved to the configuration partition (The secondary F2FS one we created earlier).
There are scripts located in /pi/sbin to help with this. 

To save your roompi.conf file run ```/pi/sbin/saveconfig```. This command will also save any changes you have made to your root password.

To save your wireless configuration, run ```/pi/sbin/savetoflash /etc/wpa_supplicant.conf```

Now you can reboot and the device should automatically connect to your wireless network and begin reporting to the roompi website. Your final step is to approve your device on the website as it will be classed as an Unauthorised device until you have manually approved it.
