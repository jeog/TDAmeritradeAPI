RM := rm -rf
LIBS := -lssl -lcrypto -lz -lcurl -lpthread -lutil -ldl
UNAME := $(shell uname -s)
ifeq ($(UNAME), Darwin)
	LIBS += -luv
endif

TDMA_SRCDIRS := $(sort $(dir $(wildcard ../src/*/)) )
UWS_SRCDIRS := $(sort $(dir $(wildcard ../uWebSockets/*/)) )
SRCDIRS := $(TDMA_SRCDIRS) $(UWS_SRCDIRS)

TDMA_OBJ_DIRS := $(subst ../,,$(TDMA_SRCDIRS))
UWS_OBJ_DIRS := $(subst ../,,$(UWS_SRCDIRS))

OBJS := $(foreach var, $(SRCDIRS), $(patsubst ../%.cpp, %.o, $(wildcard $(var)*.cpp)) ) 
DEPS := $(patsubst %.o, %.d, $(OBJS))

all: libTDAmeritradeAPI.so

# Tool invocations
libTDAmeritradeAPI.so: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -shared -o "libTDAmeritradeAPI.so" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

src/%.o: ../src/%.cpp | $(TDMA_OBJ_DIRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DTHIS_EXPORTS_INTERFACE -DNDEBUG -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo "$@"
	@echo 'Finished building: $<'
	@echo ' '

uWebSockets/%.o: ../uWebSockets/%.cpp | $(UWS_OBJ_DIRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DTHIS_EXPORTS_INTERFACE -DNDEBUG -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

$(TDMA_OBJ_DIRS):
	mkdir -p $@

$(UWS_OBJ_DIRS):
	mkdir -p $@

-include $(DEPS)

# Other Targets
clean:
	-$(RM) $(OBJS) $(DEPS) libTDAmeritradeAPI.so
	-@echo ' '

.PHONY: all clean dependents

