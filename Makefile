TARGET = CameraManager

CC = g++

ODIR = obj
SDIR = src

INCLUDE = -Iinclude -I/home/larsat/Qt/5.4/gcc_64/include \
    -I/home/larsat/Qt/5.4/gcc_64/include/QtCore \
    -I/home/larsat/Qt/5.4/gcc_64/include/QtGui \
    -I/home/larsat/Qt/5.4/gcc_64/include/QtWidgets\
    -I/home/larsat/Qt/5.4/gcc_64/include/QtOpenGL\
    -I/usr/include/GL/\
    
LIBS =-lpthread -lflycapture -lQtCore -lQtGui -lQtOpenGL -lQtThread
PARAMS =-std=c++11 -pthread -Wall
OBJS = $(patsubst %,$(ODIR)/%,*)


CPP_FILES := $(wildcard $(SDIR)/*.cpp)
OBJ_FILES := $(addprefix $(ODIR)/,$(notdir $(CPP_FILES:.cpp=.o)))

$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $(OBJ_FILES) $(LIBS) $(COMMON_LIBS) 

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(LIBS) $(PARAMS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ_FILES)	@echo "all cleaned up!"
