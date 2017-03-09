

import struct
import wave
from sys import argv
import matplotlib.animation as animation
import matplotlib.pyplot as plt
import numpy as np
import pyaudio
import serial

SAVE = 0.0
TITLE = ''
WIDTH = 1280
HEIGHT = 720
FPS = 25.0

nFFT = 1024
BUF_SIZE = 4 * nFFT
FORMAT = pyaudio.paInt16
CHANNELS = 2
RATE = 44100

if(len(argv)<2):
    print("Usage- python %s COM_PORT"%(argv[0]))
    exit(0)

com_port=argv[1]
print "Using COM port- " + com_port
ser = serial.Serial(com_port, 115200)#setting up the serialcommuication b/w python and COM port


def animate(i, line, stream, wf, MAX_y):
  # Read n*nFFT frames from stream, n > 0
  N = max(stream.get_read_available() / nFFT, 1) * nFFT
  data = stream.read(N)
  if SAVE:
    wf.writeframes(data)

  # Unpack data, LRLRLR...
  y = np.array(struct.unpack("%dh" % (N * CHANNELS), data)) / MAX_y
  y_L = y[::2]
  y_R = y[1::2]

  Y_L = np.fft.fft(y_L, nFFT)
  Y_R = np.fft.fft(y_R, nFFT)

  # Sewing FFT of two channels together, DC part uses right channel's
  Y = abs(np.hstack((Y_L[-nFFT / 2:-1], Y_R[:nFFT / 2])))
  log_Y = np.log10(Y)
  power_Y = 20*log_Y
  power_Avg = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
  for i in range(0,13):
    power_Avg[i+16] = np.mean(power_Y[ 511+i*5 : 511+5*i+5 ])
    power_Avg[15-i] = np.mean(power_Y[ 511-i*5-4 : 511-5*i+1 ])
  power_Avg[0] =  np.mean(power_Y[ 0 : 252])
  power_Avg[1] =  np.mean(power_Y[ 252 : 383])
  power_Avg[2] =  np.mean(power_Y[ 383 : 448])
  power_Avg[29] = np.mean(power_Y[ 576 : 642])
  power_Avg[30] = np.mean(power_Y[ 642 : 772])
  power_Avg[31] = np.mean(power_Y[ 772 : 1023])
  power_str = str("")
  if(ser.readline().rstrip() == 't'):
      for i in range(0,32):
        if(power_Avg[i] < 0):
            power_Avg[i] = 0
        else:
            power_Avg[i] = int(power_Avg[i] / 2);
            if(power_Avg[i] > 16):
                power_Avg[i] = 16
        power_str = power_str+str(power_Avg[i])+str(" ");
  ser.write(',');
  ser.write(power_str)
  print power_str
  print "\n"
  ser.write('\n')
  line.set_ydata(Y)
  return line,


def init(line):

  # This data is a clear frame for animation
  line.set_ydata(np.zeros(nFFT - 1))
  return line,


def main():

  dpi = plt.rcParams['figure.dpi']
  plt.rcParams['savefig.dpi'] = dpi
  plt.rcParams["figure.figsize"] = (1.0 * WIDTH / dpi, 1.0 * HEIGHT / dpi)

  fig = plt.figure()

  # Frequency range
  x_f = 1.0 * np.arange(-nFFT / 2 + 1, nFFT / 2) / nFFT * RATE
  print x_f
  ax = fig.add_subplot(111, title=TITLE, xlim=(x_f[0], x_f[-1]),
                       ylim=(0, 2 * np.pi * nFFT ** 2 / RATE))
  ax.set_yscale('symlog', linthreshy=nFFT ** 0.5)
  line, = ax.plot(x_f, np.zeros(nFFT - 1))

  # Change x tick labels for left channel
  def change_xlabel(evt):
    labels = [label.get_text().replace(u'\u2212', '')
              for label in ax.get_xticklabels()]
    ax.set_xticklabels(labels)
    fig.canvas.mpl_disconnect(drawid)
  drawid = fig.canvas.mpl_connect('draw_event', change_xlabel)
  p = pyaudio.PyAudio()
  # Used for normalizing signal. If use paFloat32, then it's already -1..1.
  # Because of saving wave, paInt16 will be easier.
  MAX_y = 2.0 ** (p.get_sample_size(FORMAT) * 8 - 1)

  frames = None
  wf = None
  if SAVE:
    frames = int(FPS * SAVE)
    wf = wave.open('temp.wav', 'wb')
    wf.setnchannels(CHANNELS)
    wf.setsampwidth(p.get_sample_size(FORMAT))
    wf.setframerate(RATE)

  stream = p.open(format=FORMAT,
                  channels=CHANNELS,
                  rate=RATE,
                  input=True,
                  frames_per_buffer=BUF_SIZE)

  ani = animation.FuncAnimation(
    fig, animate, frames,
    init_func=lambda: init(line), fargs=(line, stream, wf, MAX_y),
    interval=1000.0 / FPS, blit=True
  )

  if SAVE:
    ani.save('temp.mp4', fps=FPS)
  else:
    plt.show()

  stream.stop_stream()
  stream.close()
  p.terminate()

  if SAVE:
    wf.close()


if __name__ == '__main__':
  main()
