import tornado.ioloop
import tornado.iostream
import tornado.gen
import sys
import asyncio
import socket
import traceback
import json
import logger

SIGLENT_SINGLE_SHOT_DATA_COUNT = 751
DEFAULT_FREQUENCIES = ["10000","50000","100000", "150000", "500000", "1000000", "5000000", "10000000", "20000000", "30000000"]


class FrequencyAnalyser:
    ip = "192.168.0.8"
    port = 5025
    frequencies = []

    def __init__(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
        self.stream = tornado.iostream.IOStream(s)

    async def connect(self):
        logger.ILine(f"connect begin {ip} {port}")
        try:
            rt = await self.stream.connect((self.ip, self.port))
        except tornado.iostream.StreamClosedError as e:
            logger.DLine(f"failed to connect")
            return False
        logger.ILine(f"connect begin")
        return True

    async def get(self, statement):
    	if not statement or not isinstance(statement, str):
    		return False
    	if statement[-1] is not '\n':
            statement = statement + "\n"
        await self.stream.write(statement.encode())
        data = await self.stream.read_until(b"\n")
        return data.decode().rstrip()

    async def set(self, statement):
    	if not statement or not isinstance(statement, str):
    		return False
    	if statement[-1] is not '\n':
            statement = statement + "\n"
        await self.stream.write(statement.encode())
        return True

    async def get_system_datetime(self):
        await self.stream.write(b":SYSTem:DATE?\n")
        data = await self.stream.read_until(b"\n")
        da = data.decode().rstrip()
        await self.stream.write(b":SYSTem:TIME?\n")
        data = await self.stream.read_until(b"\n")
        ti = data.decode().rstrip()
        return f"{da} {ti}"

    async def get_system_information(self):
        logger.ILine(f"get_network_information begin")
        await self.stream.write(b"*IDN?\n")
        data = await self.stream.read_until(b"\n")
        idn = data.decode().rstrip()
        logger.DLine(f"device information: {idn}")

        await self.stream.write(b":SYSTem:CONFigure:SYSTem?\n")
        data = await self.stream.read_until(b"\n")
        sysinfo = data.decode().rstrip()
        logger.DLine(f"device system info: {sysinfo}")

        await self.stream.write(b":SYSTem:PON:TYPE?\n")
        data = await self.stream.read_until(b"\n")
        pon = data.decode().rstrip()
        logger.DLine(f"power type: {pon}")

        await self.stream.write(b":SYSTem:OPTions?\n")
        data = await self.stream.read_until(b"\n")
        sysopts = data.decode().rstrip()

        logger.DLine(f"install option: {sysopts}")
        logger.ILine(f"get_system_information end {idn}")
        return idn, sysinfo, pon, sysopts


    async def get_network_information(self):
        logger.ILine(f"get_network_information begin")
        await self.stream.write(b":SYSTem:COMMunicate:LAN:IPADdress?\n")
        data = await self.stream.read_until(b"\n")
        ip = data.decode().rstrip()

        await self.stream.write(b":SYSTem:COMMunicate:LAN:SMASk?\n")
        data = await self.stream.read_until(b"\n")
        mask = data.decode().rstrip()

        await self.stream.write(b":SYSTem:COMMunicate:LAN:GATeway?\n")
        data = await self.stream.read_until(b"\n")
        gw = data.decode().rstrip()

        await self.stream.write(b":SYSTem:COMMunicate:LAN:TYPE?\n")
        data = await self.stream.read_until(b"\n")
        lant = data.decode().rstrip()

        network_info = ip, mask, gw, lant
        logger.ILine(f"get_network_information end {network_info}")
        return ip, mask, gw, lant


    async def set_system_parameters(self):
        logger.ILine(f"set_system_parameters begin")

        await self.stream.write(b":INSTrument:SELect SA\n")
        logger.ILine(f"select instrument as Specturm Analyser mode")
        await self.stream.write(b":INSTrument:SELect?\n")
        data = await self.stream.read_until(b"\n")
        logger.ILine(f"instrument mode: {data.decode().rstrip()}")

        # set ZERO frequency span mode
        await self.stream.write(b":FREQuency:SPAN:ZERO\n")
        await self.stream.write(b":FREQuency:SPAN?\n")
        data = await self.stream.read_until(b"\n")
        span = data.decode().rstrip()
        logger.ILine(f"frequency span: {span}")

        await self.stream.write(b":FREQuency:CENTer:STEP:AUTO OFF\n")
        logger.ILine(f"auto frequency step:")
        await self.stream.write(b":FREQuency:CENTer:STEP:AUTO?\n")
        data = await self.stream.read_until(b"\n")
        logger.ILine(f"  {data.decode().rstrip()}")

        await self.stream.write(f":UNIT:POWer DBMV\n".encode())
        await self.stream.write(b":UNIT:POWer?\n")
        data = await self.stream.read_until(b"\n")
        unit = data.decode().rstrip()
        logger.ILine(f"power unit: {unit}")
        logger.ILine(f"set_general_parameters end")

        await self.stream.write(b":TRACe:MODE WRITe\n")
        await self.stream.write(b":TRACe:MODE?\n")
        data = await self.stream.read_until(b"\n")
        mode = data.decode().rstrip()
        logger.ILine(f"trace display mode: {mode}")

        await self.stream.write(b":FORMat:TRACe:DATA ASCii\n")
        await self.stream.write(b":FORMat:TRACe:DATA?\n")
        data = await self.stream.read_until(b"\n")
        fmt = data.decode().rstrip()
        logger.ILine(f"trace data format: {fmt}")

        logger.ILine(f"set_system_parameters end")

    async def set_scan_parameter(self, freq):
        logger.ILine(f"set_scan_parameter begin")

        bwd = 100000
        if isinstance(freq, int):
            if freq <= 150000:
                bwd = 300
            elif freq <= 30000000:
                bwd = 3000
            elif freq <= 1000000000:
                bwd = 10000
            else:
                bwd = 30000

        await self.stream.write(f":BWIDth {bwd}\n".encode())
        await self.stream.write(b":BWIDth?\n")
        data = await self.stream.read_until(b"\n")
        bwidth = data.decode().rstrip()
        logger.DLine(f"bwidth: {bwidth}")

        await self.stream.write(f":FREQuency:CENTer {freq}\n".encode())
        await self.stream.write(b":FREQuency:CENTer?\n")
        data = await self.stream.read_until(b"\n")
        center = data.decode().rstrip()
        logger.DLine(f"frequency center: {center}")

        # await self.stream.write(f":FREQuency:STARt {freq}\n".encode())
        # await self.stream.write(b":FREQuency:STARt?\n")
        # data = await self.stream.read_until(b"\n")
        # start = data.decode().rstrip()
        # logger.DLine(f"frequency start: {start}")

        # await self.stream.write(f":FREQuency:STOP {freq}\n".encode())
        # await self.stream.write(b":FREQuency:STOP?\n")
        # data = await self.stream.read_until(b"\n")
        # stop = data.decode().rstrip()
        # logger.DLine(f"frequency stop: {stop}")
        logger.ILine(f"set_scan_parameter end")


    async def set_frequency_parameters(self, f_center, f_bw, f_start, f_stop):
        await self.stream.write(f":FREQuency:CENTer {f_center}\n".encode())
        logger.DLine(f"set center frequency:")

        await self.stream.write(b":FREQuency:CENTer?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"  {data.decode().rstrip()}")

        await self.stream.write(f":BWIDth {f_bw}\n".encode())
        await self.stream.write(b":BWIDth?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"BWD: {data.decode().rstrip()}")

        # await self.stream.write(b":FREQuency:CENTer:STEP:AUTO ON\n")
        logger.DLine(f"auto frequency step:")
        await self.stream.write(b":FREQuency:CENTer:STEP:AUTO?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"  {data.decode().rstrip()}")

        await self.stream.write(f":FREQuency:STARt {f_start}\n".encode())
        logger.DLine(f"set frequency start:")
        await self.stream.write(b":FREQuency:STARt?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"  {data.decode().rstrip()}")

        await self.stream.write(f":FREQuency:STOP {f_stop}\n".encode())
        logger.DLine(f"set frequency stop:")
        await self.stream.write(b":FREQuency:STOP?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"  {data.decode().rstrip()}")

        logger.DLine("trace scale type:")
        await self.stream.write(b":DISPlay:WINDow:TRACe:X:SPACing LOGarithmic\n")
        await self.stream.write(b":DISPlay:WINDow:TRACe:X:SPACing?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"  X: {data.decode().rstrip()}")

        await self.stream.write(b":DISPlay:WINDow:TRACe:Y:SPACing LINear\n")
        await self.stream.write(b":DISPlay:WINDow:TRACe:Y:SPACing?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"  Y: {data.decode().rstrip()}")

    async def get_control_information(self):
        await self.stream.write(b":CORRection:IMPedance?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"impedance: {data.decode().rstrip()}")

        await self.stream.write(b":DETector:TRACe1?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"detector type: {data.decode().rstrip()}")

        await self.stream.write(b":AVERage:TYPE?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"average type: {data.decode().rstrip()}")

        await self.stream.write(b":AVERage:TRACe1:COUNt?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"average count: {data.decode().rstrip()}")

        await self.stream.write(b":SWEep:MODE?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"swep mode: {data.decode().rstrip()}")

        await self.stream.write(b":SWEep:TIME?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"swep time: {data.decode().rstrip()}")

        await self.stream.write(b":SWEep:TIME:AUTO?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"auto swep time mode: {data.decode().rstrip()}")

        await self.stream.write(b":SWEep:SPEed?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"swep speed: {data.decode().rstrip()}")

        await self.stream.write(b":SWEep:COUNt?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"swep count: {data.decode().rstrip()}")

        await self.stream.write(b":QPD:DWELl:TIME?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"QPD time: {data.decode().rstrip()}") 


        await self.stream.write(b":CAT:FREFlect:TYPE?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"calibration type: {data.decode().rstrip()}") 

        await self.stream.write(b":OUTPut:STATe?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"TG state: {data.decode().rstrip()}") 

        await self.stream.write(b":SOURce:POWer?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"TG power: {data.decode().rstrip()}") 

        await self.stream.write(b":SOURce:CORRection:OFFSet?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"TG correction: {data.decode().rstrip()}") 

        await self.stream.write(b":DISPlay:WINDow:TRACe:Y:SCALe:NRLevel?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"TG NRLevel: {data.decode().rstrip()}") 

        await self.stream.write(b":DISPlay:WINDow:TRACe:Y:SCALe:NRPosition? \n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"TG NRPosition: {data.decode().rstrip()}") 

        await self.stream.write(b":DISPlay:WINDow:NTTRace:STATe?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"TG NTTRace: {data.decode().rstrip()}")

        await self.stream.write(b":DEMod?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"DEMod: {data.decode().rstrip()}")

        await self.stream.write(b":DEMod:TIME?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"DEMod time: {data.decode().rstrip()}")

        await self.stream.write(b":DEMod:VOLume?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"DEMod volumn: {data.decode().rstrip()}")

        await self.stream.write(b":CALCulate:MATH:FUNCtion?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f"trace calculate type: {data.decode().rstrip()}")

        logger.DLine(f"trace math variables:")
        await self.stream.write(b":TRACe:MATH:X?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f" X: {data.decode().rstrip()}")
        await self.stream.write(b":TRACe:MATH:Y?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f" Y: {data.decode().rstrip()}")
        await self.stream.write(b":TRACe:MATH:Z?\n")
        data = await self.stream.read_until(b"\n")
        logger.DLine(f" Z: {data.decode().rstrip()}")


    async def initiate(self):
        logger.ILine(f"initiate")
        await self.stream.write(b":INITiate:CONTinuous OFF\n")
        await self.stream.write(b":INITiate:IMMediate\n")

    async def inquire(self):
        logger.ILine(f"begin inquire")
        await self.stream.write(b"*OPC?\n")
        data = await self.stream.read_until(b"\n")
        opc = data.decode().rstrip()
        logger.ILine(f"end inquire {opc}")
        return int(opc) == 1


    async def acquire(self):
        logger.ILine(f"begin acquire")
        # 751 data
        await self.stream.write(b":TRACe:DATA?\n")
        data = await self.stream.read_until(b"\n")
        data = data.decode().rstrip()
        logger.DLine(f"trace data: {data}")
        raw_datapoints = [float(datum) for datum in data.split(",")]
        # avgv = sum(raw_datapoints)/len(raw_datapoints)
        mv = max(raw_datapoints)
        logger.ILine(f"end acquire {mv}")
        return mv

    async def poweroff(self):
        await self.stream.write(b":SYSTem:POWer:OFF\n")

    def close(self):
        self.stream.close()

async def get_data(statement):
    logger.ILine(f"get_data begin {statement}")
    fa = FrequencyAnalyser()
    try:
        rt = await fa.connect()
        if rt:
        	return fa.get(statement)
        else:
            logger.ELine("failed to connect!")
    finally:
        fa.close()
        logger.ILine(f"get_data end")

async def set_data(statement):
    logger.ILine(f"set_data begin {statement}")
    fa = FrequencyAnalyser()
    try:
        rt = await fa.connect()
        if rt:
        	return fa.set(statement)
        else:
            logger.ELine("failed to connect!")
    finally:
        fa.close()
        logger.ILine(f"set_data end")

async def query_info():
    logger.ILine(f"query_info begin")
    fa = FrequencyAnalyser()
    try:
        rt = await fa.connect()
        if rt:
            logger.ILine("connected successfully")
            net_info = await fa.get_network_information()
            sys_info = await fa.get_system_information()
            return list(net_info + sys_info)
        else:
            logger.ELine("failed to connect!")
    finally:
        fa.close()
        logger.ILine(f"query_info end")


async def meter_once(frequencies=[]):
    logger.ILine(f"meter_once begin {frequencies}")
    fa = FrequencyAnalyser()
    try:
        rt = await fa.connect()
        if rt:
            logger.ILine("connected successfully")
            await fa.set_system_parameters()          
            logger.ILine("configuration is ok")
            await tornado.gen.sleep(0.5)
            fa.frequencies = [int(f) for f in frequencies if frequencies else DEFAULT_FREQUENCIES]
            datapoints = []
            for fq in fa.frequencies:
                logger.ILine(f"meter for {fq}Hz begin")
                await fa.set_scan_parameter(fq)
                await tornado.gen.sleep(0.1)
                await fa.initiate()
                await tornado.gen.sleep(0.1)
                if await fa.inquire():
                    datapoints.append(await fa.acquire())
                    logger.ILine(f"meter for {fq}Hz end")
                else:
                    datapoints.append(None)
                    logger.ILine(f"meter for {fq}Hz failed!")
            logger.ILine(f"datapoints: {datapoints}")
            # return float array
            return datapoints
        else:
            logger.ELine("failed to connect!")
    finally:
        fa.close()
        logger.ILine(f"meter_once end")

def main():  
    for line in sys.stdin:
        try:
            # read JSON
            data = json.loads(line.strip())
            if isinstance(data, dict) and (data.get("command") == "get"):
	            result = asyncio.run(get_data())
	            response = {
	                "status": "ok",
	                "result": result
	            }
	            print(json.dumps(response))
	            sys.stdout.flush()

            if isinstance(data, dict) and (data.get("command") == "set"):
	            result = asyncio.run(set_data())
	            response = {
	                "status": "ok"
	            }
	            print(json.dumps(response))
	            sys.stdout.flush()

            if isinstance(data, dict) and (data.get("command") == "query"):
	            result = asyncio.run(query_info())
	            response = {
	                "status": "ok",
	                "result": result
	            }
	            print(json.dumps(response))
	            sys.stdout.flush()

            if isinstance(data, dict) and (data.get("command") == "meter"):
	            frequencies = data.get("param")
	            result = asyncio.run(meter_once(frequencies))
	            response = {
	                "status": "ok",
	                "result": result
	            }
	            print(json.dumps(response))
	            sys.stdout.flush()  # 确保输出被Qt读取

        except json.JSONDecodeError as e:
            response = {
                "status": "false",
                "result": f"Invalid JSON: {e}"
            }
            print(json.dumps(response))
            sys.stdout.flush()
 
if __name__ == "__main__":  
    logger.start('warning')
    main()
