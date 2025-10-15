#!/bin/bash

# This script simulates a long-running process that writes its status
# to a log file every second.

# Define the log file name
LOG_FILE="my_job.log"
DURATION_SECONDS=20

# Print a message to the console and start the log file
echo "Starting background job. PID is $$."
echo "--- Job started at $(date) (PID: $$) ---" > "$LOG_FILE"
echo "This script will run for approximately $DURATION_SECONDS seconds." >> "$LOG_FILE"

# Simulate work by looping and sleeping
for ((i=1; i<=$DURATION_SECONDS; i++)); do
  echo "[$(date +%H:%M:%S)] Working... $(($i * 10))% complete." >> "$LOG_FILE"
  # Pause for 1 second to simulate processing time
  sleep 1
done

# Final message indicating the job is finished
echo "--- Job finished successfully at $(date) ---" >> "$LOG_FILE"

# Exit cleanly. When this line is reached, the process with PID $$ terminates.
exit 0