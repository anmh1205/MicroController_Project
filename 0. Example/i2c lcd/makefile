# makefile by Ngo Hung Cuong
# pic16f887
# compiler xc8

CC = "C:/Program Files/Microchip/xc8/v2.20/bin/xc8-cc.exe"
PIC_MCU = 16f887
OUT = out

TARGET = i2c_lcd

# source code
SRC = 
SRC += main.c
SRC += i2c.c
SRC += i2c_lcd.c

OBJ = $(patsubst %.c, %.p1, $(SRC))
OBJ_PARAM = $(patsubst %.c, $(OUT)/%.p1, $(SRC))

# include folder

INC = 
INC += .

INCS = $(patsubst %, -I%, $(INC))

all: $(OBJ_PARAM)
	$(CC) -mcpu=$(PIC_MCU) -std=c90 $(OBJ_PARAM) -o $(OUT)/$(TARGET).hex
	
$(OUT)/%.p1: %.c
	$(CC) -mcpu=$(PIC_MCU) -std=c90 -c $(INCS) $< -o $@

clean:
	rm -f $(OUT)/*.*