#ifndef main_h
#define main_h

#include "uinput.h"
#include "libcec.h"
#include <limits.h>
#include <string>
#include <queue>
#include <list>
#include <log4cplus/logger.h>

class Command
{
	public:
		Command(int command, CEC::cec_user_control_code keycode=CEC::CEC_USER_CONTROL_CODE_UNKNOWN) : command(command), keycode(keycode) {};
		~Command() {};

		const int command;
		union
		{
			const CEC::cec_user_control_code keycode;
		};

};

class Main : public CecCallback {

	private:

		// Main controls
		Cec cec;
		UInput uinput;
		char cec_name[HOST_NAME_MAX];

		// Some config params
		bool makeActive;
		bool running; // TODO Change this to be threadsafe!. Voiatile or better

		//
		std::list<__u16> lastUInputKeys; // for key(s) repetition

		//
		Main();
		virtual ~Main();

		// Not implemented to avoid copying the singleton
		Main(Main const&);
		void operator=(Main const&);

		static void signalHandler(int sigNum);

		static const std::map<const std::string, int> setupKeyMap();
    static std::vector<std::list<__u16>> setupUinputMap();
    static std::map<const std::string, std::list<std::string>> setupConfigMap();

		std::queue<Command> commands;

		std::string onStandbyCommand;
		std::string onActivateCommand;
		std::string onDeactivateCommand;

		CEC::cec_logical_address logicalAddress;

		char *getCecName();

		void push(Command command);

	public:
        
    static const std::map<const std::string, int> uinputKeyMap;
    static std::vector<std::list<__u16>> uinputCecMap;
    static std::map<const std::string, std::list<std::string>> configMap;
    static log4cplus::Logger logger;
        
		std::list<__u16> lookupCecUinputMapping(CEC::cec_user_control_code);
    std::list<std::string> lookupConfigMappings(std::string);
        
		int onCecLogMessage(const CEC::cec_log_message &message);
		int onCecKeyPress(const CEC::cec_keypress &key);
		int onCecKeyPress(const CEC::cec_user_control_code & keycode);
		int onCecCommandReceived(const CEC::cec_command &command);
		int onCecConfigurationChanged(const CEC::libcec_configuration & configuration);
		int onCecAlert(const CEC::libcec_alert alert, const CEC::libcec_parameter & param);
		int onCecMenuStateChanged(const CEC::cec_menu_state & menu_state);
		void onCecSourceActivated(const CEC::cec_logical_address & address, bool isActivated);

		static Main & instance();

		void loop(const std::string &device = "");
		void stop();
		void restart();

		void listDevices();

		void setMakeActive(bool active) {this->makeActive = active;};

		void setOnStandbyCommand(const std::string &cmd) {this->onStandbyCommand = cmd;};
		void setOnActivateCommand(const std::string &cmd) {this->onActivateCommand = cmd;};
		void setOnDeactivateCommand(const std::string &cmd) {this->onDeactivateCommand = cmd;};
		void setTargetAddress(const HDMI::address & address) {cec.setTargetAddress(address);};
};

#endif //main_h
