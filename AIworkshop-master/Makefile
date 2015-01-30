SRC := pyagent.cpp cagent.cpp main.cpp utility.cpp sfmldrawer.cpp environment.cpp entities.cpp
FILES := $(addprefix src/, $(SRC))
PYTHON_FLAGS := -I/usr/include/python3.2mu -I/usr/include/python3.2mu -DNDEBUG -g -fwrapv -D_FORTIFY_SOURCE=2 -g -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security
PYTHON_LINK := -L/usr/lib/python3.2/config-3.2mu -lpthread -ldl -lutil -lm -lpython3.2mu -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions
CPPFLAGS := -std=c++11 -g -o ./bin/ai
SFML_LINK := -lsfml-graphics -lsfml-window -lsfml-system
all:
	g++ $(PYTHON_FLAGS) $(CPPFLAGS) $(FILES) $(SFML_LINK) $(PYTHON_LINK)