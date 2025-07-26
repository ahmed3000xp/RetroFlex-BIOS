SRC_DIR = src

all: clean bios qemu

bios:
	$(MAKE) -C $(SRC_DIR) bios

qemu:
	$(MAKE) -C $(SRC_DIR) qemu

qemu_kvm:
	$(MAKE) -C $(SRC_DIR) qemu_kvm

cross:
	$(MAKE) -C $(SRC_DIR) cross

clean:
	$(MAKE) -C $(SRC_DIR) clean
