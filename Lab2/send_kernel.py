from serial import Serial

def getBinary(x):
    ret = ''
    c01 = "01"
    while x > 0:
        ret += c01[x % 2]
        x >>= 1
    while len(ret) < 32:
        ret += c01[0]
    return ret[::-1].encode()

with open('build/kernel.bin', 'rb') as f:
    raw = f.read()
    print(type(raw))
    length = len(raw)
    print(f"Kernel size: {getBinary(length)}({length})")
    print(raw[0])
    print(bytes(raw[0]))
    print(raw[0:1])
    with Serial("/dev/ttyUSB0", 115200) as ser:
        #send kernel size in binary
        print("Start to send kernel size to UART in binary.")
        ser.write(getBinary(length))
        ser.flush()
        print("OK.")
        
        print("Start to send kernel to UART in byte.")
        for i in range(length):
            ser.write(raw[i:i+1])
            ser.flush()
            print(f"{i+1}/{length} bytes sent.")
        print("OK")

