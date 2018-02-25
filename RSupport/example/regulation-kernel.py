import sys

sys.path.append("/usr/local/share/python3/")

import RRegistry as RR
import RSupport as RS

rsupport = RS.RSupport()

print("Created handle! Trying to connect")

# Connect to default path.
status = rsupport.connect("/tmp/lrt_pipe_path.pipe")
if status != RS.RSupportStatus_Ok:
    print("Error while connecting: " + RS.rsupport_status_msg(status))

# After connecting, options can be set.

# The frequency should be regulated automatically.
rsupport.setOption(RS.RSupportOption_AutoFrequency, True)
# After each loop, the movement state should be
# forwarded to the hardware and the Arduino.
rsupport.setOption(RS.RSupportOption_AutoMovementBurst, True)

# Receive the registry instance.
registry = rsupport.registry()

# Subscribe to inputs.
rsupport.subscribe(RR.Type_Int16, RR.Int16_MVMT_STEER_DIRECTION)
rsupport.subscribe(RR.Type_Int16, RR.Int16_MVMT_FORWARD_VELOCITY)

# Start loop.
while(True):
    # Service the Regulation Kernel for newest updates and synchronisation.
    rsupport.service()

    # Get the needed variables from the local copy of the registry.
    # The properties of these variables can be looked up in the wiki.
    forward_velocity = registry.getInt16(RR.Int16_MVMT_FORWARD_VELOCITY)
    steer_direction = registry.getInt16(RR.Int16_MVMT_STEER_DIRECTION)

    forward_velocity /= 128;
    steer_direction /= 256;

    if forward_velocity < -255:
        forward_velocity = -255
    if steer_direction < -255:
        steer_direction = -255
    if forward_velocity > 255:
        forward_velocity = 255
    if steer_direction > 255:
        steer_direction = 255

    # Run the calculations.
    motor_fl = int(forward_velocity)
    motor_fr = int(forward_velocity)
    motor_rl = int(forward_velocity)
    motor_rr = int(forward_velocity)

    servo_fl = steer_direction + 128
    servo_fr = steer_direction + 128
    servo_rl = steer_direction + 128
    servo_rr = steer_direction + 128

    # Assign the calculated variables into the registry.
    registry.setInt16(RR.Int16_MVMT_MOTOR_PWM_FL, int(motor_fl))
    registry.setInt16(RR.Int16_MVMT_MOTOR_PWM_FR, int(motor_fr))
    registry.setInt16(RR.Int16_MVMT_MOTOR_PWM_RL, int(motor_rl))
    registry.setInt16(RR.Int16_MVMT_MOTOR_PWM_RR, int(motor_rr))

    registry.setUint8(RR.Uint8_MVMT_SERVO_FL_POSITION, int(servo_fl))
    registry.setUint8(RR.Uint8_MVMT_SERVO_FR_POSITION, int(servo_fr))
    registry.setUint8(RR.Uint8_MVMT_SERVO_RL_POSITION, int(servo_rl))
    registry.setUint8(RR.Uint8_MVMT_SERVO_RR_POSITION, int(servo_rr))
