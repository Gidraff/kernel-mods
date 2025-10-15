# Kernel modification

This repository is a C/C++ playground for Kernel and Device drivers.

#### Set up guide
Make sure you have go and Make installed. 

Running `make` will use the kernel build system to compile out driver to kernel object. 

To load the Kernel module, we need to run:
```
sudo insmod my_device.ko 2>&1

# Checks for information about the module
sudo modinfo my_device.ko 
```

Now, we will compile our go program, the code that uses out module above. To do that, we run:
```
go build -o my_dev_app main.go
```

To test our module using the go app, run this command

Write:
```
./my_dev_app write "Hello from the user space" 
```

Read:
```
./my_dev_app read
```

#### Cleaning up
To clean up, run the following command:
```
sudo rmmod my_device

make clean
```


