#pragma once

class CObject;

class State {
public:
};

class Command {
public:
	virtual ~Command() {}
	virtual void Execute(CObject& object) = 0;
	//virtual void undo() = 0;
};

class ClickCommand : public Command {
public:
	void Execute(CObject& object) override;
private:
	POINT old_cursor_pos;
};

class InputManager {
public:
	InputManager();
	~InputManager();
	void ProcessInput();
private:
	UCHAR key_buffer[256];
	Command* mouse;
	CObject* object;	// 참조용
};
