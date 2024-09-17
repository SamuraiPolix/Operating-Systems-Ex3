
TARGETS = q1 q2 q3 q4 q5-6 q7

# Declare non-files
.PHONY: all clean $(TARGETS)

# Make the programs for all questions
all: $(TARGETS)

$(TARGETS): %:
	make -C $@ all

# Clean programs for all questions
clean:
	for target in $(TARGETS); do \
		make -C $$target clean; \
	done