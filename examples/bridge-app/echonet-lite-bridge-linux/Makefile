main:
	gn gen out/debug
	ninja -C out/debug
	./out/debug/chip-bridge-app
pull:
	git reset HEAD^ --hard
	git pull
	gn gen out/debug
	ninja -C out/debug
	./out/debug/chip-bridge-app
gdb:
	gn gen out/debug
	ninja -C out/debug
	gdb -ex run ./out/debug/chip-bridge-app
gdbfast:
	gn gen out/debug
	ninja -C out/debug
	gdb run --args ./out/debug/chip-bridge-app -asyncread -asyncwrite

teststartup:
	./out/debug/chip-bridge-app
	./out/debug/chip-bridge-app
	./out/debug/chip-bridge-app
	./out/debug/chip-bridge-app
	./out/debug/chip-bridge-app
	./out/debug/chip-bridge-app
	./out/debug/chip-bridge-app
	./out/debug/chip-bridge-app
	./out/debug/chip-bridge-app
	./out/debug/chip-bridge-app

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:
	gn gen out/debug