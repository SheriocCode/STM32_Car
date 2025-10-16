import asyncio, json, struct, serial, serial.tools.list_ports
from aiohttp import web
import aiohttp

clients = set()

async def uart2ws():
    """死循环：串口→WebSocket"""
    with serial.Serial('COM6', 9600, timeout=0.1) as ser:
        while True:
            block = ser.read(6)
            if len(block)==6 and block[0]==0xFF and block[-1]==0xFE:
                left  = struct.unpack('b', block[1:2])[0]
                right = struct.unpack('b', block[2:3])[0]
                dis   = block[3]
                pkg   = json.dumps({'left':left,'right':right,'dist':dis})
                # 广播
                dead = set()
                for ws in clients:
                    try: await ws.send_str(pkg)
                    except ConnectionResetError:
                        dead.add(ws)
                clients.difference_update(dead)
            await asyncio.sleep(0.02)   # 50 Hz 够用

async def websocket_handler(request):
    ws = web.WebSocketResponse()
    await ws.prepare(request)
    clients.add(ws)
    async for msg in ws: pass   # 只收不发
    clients.remove(ws)
    return ws

app = web.Application()
app.router.add_get('/ws', websocket_handler)
app.router.add_static('/', path='.', name='static')  # 托管 html/js
async def main():
    await asyncio.gather(
        web._run_app(app, host='0.0.0.0', port=8080),
        uart2ws()
    )

asyncio.run(main())