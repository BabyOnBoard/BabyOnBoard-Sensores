sudo service motion start
touch /home/pi/teste.txt
source ~/.bobenv/bin/activate
python3 /home/pi/Git/BabyOnBoard-API/babyonboard/manage.py runserver 0.0.0.0:8000
./home/pi/Git/BabyOnBoard-Sensores/temperatura &
./home/pi/Git/BabyOnBoard-Sensores/microfone &
python3 /home/pi/Git/BabyOnBoard-Sensores/respiracao.py &
touch /home/pi/teste2.txt
