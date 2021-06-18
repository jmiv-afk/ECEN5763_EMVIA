# Dead-simple python analysis script to parse stats.txt 
import re
import statistics as stat

target_fr = 30  #fps
msec = []
jitter_ms = []

file = open("stats.txt", 'r')
lines = file.readlines()
file.close()
for line in lines:
    line = line.strip()
    line = re.split(',', line)
    msec.append(float(line[2]))
    jitter_ms.append(1000/target_fr-msec[-1])

print("Total frames:", len(msec))
print("Total sec: {:5.3f}".format(sum(msec)/1000))
print("Frame Rates: ")
print("    Avg: {:5.3f} fps".format(1000*len(msec)/sum(msec)))
print("    Min: {:5.3f} fps".format(1000/max(msec)))
print("    Max: {:5.3f} fps".format(1000/min(msec)))
print("Jitter (target_ms - actual_ms): ")
print("    Avg: {:5.3f} msec".format(sum(jitter_ms)/len(jitter_ms)))
print("    Min: {:5.3f} msec".format(min(jitter_ms)))
print("    Max: {:5.3f} msec".format(max(jitter_ms)))
