
.PHONY: build
build:
	pio run
.PHONY: flash
flash:
	pio run --target upload
.PHONY: flash_wifi
flash_wifi:
	pio run -e d1_mini_wifi -t upload
