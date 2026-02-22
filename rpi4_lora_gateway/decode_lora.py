import subprocess, json, base64

proc = subprocess.Popen(
    ['journalctl','-f','-u','single_chan_pkt_fwd','-o','cat'],
    stdout=subprocess.PIPE, text=True
)

for line in proc.stdout:
    if '"rxpk"' in line:
        # pull out the JSON part
        try:
            j = json.loads(line[line.find('{'):])
            b64 = j['rxpk'][0]['data']
            text = base64.b64decode(b64).decode('utf-8')
            print(text)
        except Exception:
            continue
