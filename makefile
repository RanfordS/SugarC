CCFLAGS = -Wall -Wextra -Og -ggdb -std=c++11
LDFLAGS = -Wall -Wextra -Og -ggdb -std=c++11
LINKLIB =

OBJS = main parse_initial
HDRS = parse_header
TARG = sugarc
CCMP = g++

OBJDIR = objs

CFILES = $(addsuffix .cpp, $(OBJS))
HFILES = $(addsuffix .hpp, $(HDRS))
OFILES = $(addsuffix .o, $(addprefix $(OBJDIR)/, $(OBJS)))

$(TARG) : $(OFILES) $(HFILES)
	@echo -e '\033[1;36mLinking\033[0m'
	@$(CCMP) $(LDFLAGS) -o $(TARG) $(OFILES) $(LINKLIB)
	@echo -e '\033[1;4mDone\033[0m'

$(OBJDIR)/%.o : %.cpp $(HFILES)
	@echo -e '\033[1;33mBuilding $<\033[0m'
	@$(CCMP) $(CCFLAGS) -o $@ -c $<

.PHONY : clean
clean :
	@echo -e '\033[1;35mCleaning object files\033[0m'
	@rm -f $(OBJDIR)/*
	@rm -f $(TARG)
