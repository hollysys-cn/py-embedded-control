import sys
sys.path.insert(0, ".")
sys.path.insert(0, "./python")

print("Importing plcopen...")
from plcopen import PID
print("PID imported successfully")

print("Creating PID controller...")
pid = PID(Kp=1.0, Ki=0.1, Kd=0.05, output_min=0, output_max=100)
print("PID created successfully")
print("Test passed!")
