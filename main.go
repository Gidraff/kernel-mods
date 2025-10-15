package main

import (
	"fmt"
	"os"
	"strings"
)

// Define the path to our character device file.
// This path is created by the kernel module when it's loaded.
const devicePath = "/dev/my_device"

// Define the buffer size to match the kernel module's buffer.
// This is important for consistent read/write operations.
const bufferSize = 1024

func main() {
	// Check for command-line arguments.
	// The program expects either "read" or "write [message]".
	if len(os.Args) < 2 {
		fmt.Println("Usage: mydevice-app <read|write> [message]")
		os.Exit(1)
	}

	command := os.Args[1]

	switch command {
	case "read":
		readDevice()
	case "write":
		// Ensure a message is provided for the 'write' command.
		if len(os.Args) < 3 {
			fmt.Println("Usage: mydevice-app write \"Your message here\"")
			os.Exit(1)
		}
		message := strings.Join(os.Args[2:], " ")
		writeDevice(message)
	default:
		fmt.Println("Invalid command. Use 'read' or 'write'.")
		fmt.Println("Usage: mydevice-app <read|write> [message]")
		os.Exit(1)
	}
}

// readDevice opens the device file, reads its content, and prints it.
func readDevice() {
	// Open the device file with read/write permissions.
	// The file.Close() is deferred to ensure it's closed even if an error occurs.
	file, err := os.OpenFile(devicePath, os.O_RDWR, 0666)
	if err != nil {
		fmt.Printf("Error opening device file: %v\n", err)
		return
	}
	defer file.Close()

	// Create a buffer to hold the data read from the device.
	buffer := make([]byte, bufferSize)

	// Read data from the device into the buffer.
	n, err := file.Read(buffer)
	if err != nil {
		fmt.Printf("Error reading from device: %v\n", err)
		return
	}

	// Trim the buffer to the number of bytes read and convert to a string.
	// Print the content to the console.
	fmt.Printf("Read %d bytes from device:\n%s\n", n, string(buffer[:n]))
}

// writeDevice opens the device file, writes the provided message to it.
func writeDevice(message string) {
	// Open the device file with read/write permissions.
	file, err := os.OpenFile(devicePath, os.O_RDWR, 0666)
	if err != nil {
		fmt.Printf("Error opening device file: %v\n", err)
		return
	}
	defer file.Close()

	// Write the message as a byte slice to the device file.
	n, err := file.Write([]byte(message))
	if err != nil {
		fmt.Printf("Error writing to device: %v\n", err)
		return
	}

	// Print a confirmation message.
	fmt.Printf("Wrote %d bytes to device.\n", n)
}

