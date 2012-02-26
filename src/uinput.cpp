#include "uinput.h"

#include <cstring>
#include <iostream>

#include <errno.h>
#include <fcntl.h>
#include <linux/uinput.h>


// TODO loop all uinputs
#define UINPUT_PATH "/dev/uinput"
const char *uinput_filename[] = {"/dev/uinput", "/dev/input/uinput", "/dev/misc/uinput"};

using std::cerr;
using std::endl;

UInput::UInput(const char *dev_name) : fd(-1) {
	open();
	setup(dev_name);
	create();
}

UInput::~UInput() {
	destroy();
}

void UInput::open() {
	this->fd = ::open(UINPUT_PATH, O_WRONLY);
	if(this->fd < 0) {
		cerr << "Failed to open uinput: ";

		switch (errno) {
			case ENOENT:
				cerr << "uinput was not found. Is the uinput module loaded?" << endl;
				break;

			case EACCES:
				cerr << "Permission denied. Check you have permission to uinput." << endl;
				break;

			default:
				cerr << errno << " " << strerror(errno) << endl;
				break;
		}

		//return -1;
	}
}

void UInput::setup(const char *dev_name) {

	int ret;
	struct uinput_user_dev uidev;
	memset(&uidev, 0, sizeof(uidev));

	strncpy(uidev.name, dev_name, UINPUT_MAX_NAME_SIZE);
	uidev.id.bustype = BUS_USB;
	uidev.id.version = 1;

	ret = write(this->fd, &uidev, sizeof(uidev));
	if (ret != sizeof(uidev)) {
		cerr << "Failed to setup uinput: " << errno << " " << strerror(errno) << endl;
		//return -1;
	}

	// We only want to send keypresses
	ret = ioctl(this->fd, UI_SET_EVBIT, EV_KEY);
	ret |= ioctl(this->fd, UI_SET_KEYBIT, KEY_MUTE);
	ret |= ioctl(this->fd, UI_SET_KEYBIT, KEY_VOLUMEDOWN);
	ret |= ioctl(this->fd, UI_SET_KEYBIT, KEY_VOLUMEUP);
	ret |= ioctl(this->fd, UI_SET_KEYBIT, KEY_D);

	if (ret) {
		cerr << "Failed to setup uinput: " << errno << " " << strerror(errno) << endl;
		//return -1;
	}
}

void UInput::create() {
	int ret = ioctl(this->fd, UI_DEV_CREATE);
	if (ret) {
		cerr << "Failed to create uinput" << endl;
		//return -1;
	}

	// This sleep is here, because (for some reason) you need to wait before
	// sending you first uinput event.
	sleep(1);
}

void UInput::send_event(__u16 type, __u16 code, __s32 value) const {
	struct input_event ev;
	memset(&ev, 0, sizeof(ev));

	ev.type  = type;
	ev.code  = code;
	ev.value = value;

	int ret = write(this->fd, &ev, sizeof(ev));
	if (ret != sizeof(ev)) {
		cerr << "Failed to send_event: " << errno << " " << strerror(errno) << endl;
		//return -1;
	}
}

void UInput::sync() const {
	send_event(EV_SYN, SYN_REPORT, 0);
}


void UInput::destroy() {
	int ret = ioctl(fd, UI_DEV_DESTROY);
	if (ret) {
		cerr << "Failed to create uinput" << endl;
	}

	close(this->fd);
}