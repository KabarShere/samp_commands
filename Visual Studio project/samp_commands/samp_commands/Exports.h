/*

	This file includes functions that can be called from cleo mods. 
	Under each function there's an example of how to call it in cleo.

*/

#pragma once
#include "dllmain.h"
#include "Commands.h"
#include "Hook.h"
#include "Compatibility.h"
#include <Windows.h>



extern "C" __declspec(dllexport) void _stdcall RegisterCommand(char* prefix, DWORD label_address, CLEO_RunningScript* cleo_thread_pointer) {
	if (!Compability::sampVersion)
		Compability::Init();

	if (!origChatInputHandler)
		origChatInputHandler = Hook::AttachRelative(1 + Compability::sampVersion->hook_addrGetChatInputText, (DWORD)Hooked_GetChatInputText);
		//Hook::Samp_GetChatInputText(&origChatInputHandler, (DWORD)Hooked_GetChatInputText);

	if (!origOpcodeHandler)
		Hook::Cleo_customOpcodeHandler(&origOpcodeHandler, (DWORD)Hooked_customOpcodeHandler);


	//char msg[200];
	//sprintf(msg, "prefix = %s\nlabel_address = %d\nbase address = %d\ndiff = %d", prefix, label_address, (DWORD)cleo_thread_pointer->BaseIP, (DWORD)cleo_thread_pointer->BaseIP - label_address);
	//MessageBoxA(NULL, msg, "RegisterCommand", MB_OK | MB_TOPMOST);

	
	if (!Commands::IsRegistered(prefix)) {
		Commands::vect.push_back(new Command(
			prefix,
			(DWORD)cleo_thread_pointer->BaseIP - label_address,
			cleo_thread_pointer,
			NULL
		));
		return;
	}

	char msg[300];
	sprintf(msg, "'%s' command prefix is already registered.\ncleo_that_already_registered_it->Name[8]: %s\ncleo_that_tries_to_register_it_now->Name[8]: %s", prefix, Commands::GetCleoNameWhoRegisteredFirst(), cleo_thread_pointer->Name);
	MessageBox(NULL, msg, "samp_commands.asi - RegisterCommand", MB_OK | MB_TOPMOST);
}
/*
:register_command
if 0AA2: 31@ = load_library "samp_commands.asi"
then
	if 0AA4: 30@ = get_proc_address "_RegisterCommand@12" library 31@
	then
		//0AD1: show_formatted_text_highpriority "_RegisterCommand@12 function was ~G~found ~W~:)" time 2000
		0A9F: 29@ = current_thread_pointer

		// void _stdcall RegisterCommand(char * str, DWORD label_address, CLEO_RunningScript * cleo_thread_pointer)
		0AA5: call 30@ num_params 3 pop 0 cleo_thread_pointer 29@ label_address 1@ str 0@
	else
		0AD1: show_formatted_text_highpriority "_RegisterCommand@12 function was ~R~not found ~W~:(" time 5000
	end
	0AA3: free_library 31@
end
0AB2: ret 0
*/

extern "C" __declspec(dllexport) char* _stdcall GetLastCommandParams() {
	return Commands::last_params;
}

/*
:get_last_command_params
if 0AA2: 31@ = load_library "samp_commands.asi"
then
	if 0AA4: 30@ = get_proc_address "_GetLastCommandParams@0" library 31@
	then
		//0AD1: show_formatted_text_highpriority "_GetLastCommandParams@0 function was ~G~found ~W~:)" time 2000
		0AA7: call_function 30@ num_params 0 pop 0 _params_string_pointer 29@
		0AA3: free_library 31@

		0A8D: 28@ = read_memory 29@ size 1 virtual_protect 1
		if 28@ <> 0
		then
			0485:  return_true
			0AB2: ret 1 29@
		end
	else
		0AD1: show_formatted_text_highpriority "_GetLastCommandParams@0 function was ~R~not found ~W~:(" time 5000
	end
end
059A:  return_false
0AB2: ret 1 0
*/


// CmdRet seems very simple (and weird) but it relies on the hooked "0AB2 ret" invoking function
extern "C" __declspec(dllexport) void _stdcall CmdRet() {
	if (Commands::last->return_address) {
		Commands::last_requested_return = true;
	}
}

/*
:cmd_ret
if 0AA2: 31@ = load_library "samp_commands.asi"
then
	if 0AA4: 30@ = get_proc_address "_CmdRet@0" library 31@
	then
		//0AD1: show_formatted_text_highpriority "_CmdRet@0 function was ~G~found ~W~:)" time 2000

		0AA5: call 30@ num_params 0 pop 0
	else
		0AD1: show_formatted_text_highpriority "_CmdRet@0 function was ~R~not found ~W~:(" time 5000
	end
	0AA3: free_library 31@
end
0AB2: ret 0
*/