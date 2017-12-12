clear; 
% PULSES and RECORDING_BLOCK appropriately.
PULSES = 30;
SAMPLES_PER_RANGE_LINE = 512;%1024;



IQ_SAMPLES = SAMPLES_PER_RANGE_LINE*PULSES; %32 bits per sample
BUF = IQ_SAMPLES; 
LEN = IQ_SAMPLES*2;

%% File handles to raw data files
fida=fopen('../adc0data.dat','r');
fidb=fopen('../adc1data.dat','r');
fidc=fopen('../adc2data.dat','r');

%% Read raw data and write to a vector
aa=fread(fida,LEN,'int16');
a=aa(1:2:LEN)+j*aa(2:2:LEN);

%bb=fread(fidb,LEN,'int16');
%b=bb(1:2:LEN)+j*bb(2:2:LEN);

%cc=fread(fidc,LEN,'int16');
%c=cc(1:2:LEN)+j*cc(2:2:LEN);


%% Plot I and Q data
if 1

figure
plot(real(a(1:BUF)),'r');
hold on;
plot(imag(a(1:BUF)),'b');


end

%% Plot phase

if 0
figure
plot(unwrap(angle(a(1:BUF))));
end

%% Plot a basic FFT of the data
if 0
	
flen=length(a);

vert = 20*log10(flen)+20*log10(2^16)-21; % adjust vertical scale for any length FFT and 16 bit data

    vert = 167;
    figure
    plot((1/flen)*[0:flen-1],20*log10(abs(fftshift(fft(a.*hanning(flen)))))-vert);
    bottom=-150;
    ax=axis;
    ax(1)=0;
    ax(2)=1;
    ax(3)=bottom;
    ax(4)=10;
    grid on;
    scale=bottom:10:0;
    set(gca,'ytick',scale);
    axis(ax);

end;

fclose(fida);
fclose(fidb);
fclose(fidc);


