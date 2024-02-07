#include <numbers>

void FadeOutGame(
	bool a_fadingOut,
	bool a_blackFade,
	float a_fadeDuration,
	bool a_arg4,
	float a_secsBeforeFade)
{
	using func_t = decltype(&FadeOutGame);
	REL::Relocation<func_t> func{ REL::RelocationID(51909, 52847)};
	return func(a_fadingOut, a_blackFade, a_fadeDuration, a_arg4, a_secsBeforeFade);
}

bool resetCapture = false;
bool rotateCamera = false;
int captureStep = 1;

void DrawDeferred()
{
	if (resetCapture){
		resetCapture = false;
		rotateCamera = true;
		FadeOutGame(false, true, 0.5f, true, 0.5f);
	}
}

RE::NiMatrix3 MatrixFromAxisAngle(const RE::NiPoint3& axis, float theta)
{
	RE::NiPoint3 a = axis;
	float cosTheta = cosf(theta);
	float sinTheta = sinf(theta);
	RE::NiMatrix3 result;

	result.entry[0][0] = cosTheta + a.x * a.x * (1 - cosTheta);
	result.entry[0][1] = a.x * a.y * (1 - cosTheta) - a.z * sinTheta;
	result.entry[0][2] = a.x * a.z * (1 - cosTheta) + a.y * sinTheta;

	result.entry[1][0] = a.y * a.x * (1 - cosTheta) + a.z * sinTheta;
	result.entry[1][1] = cosTheta + a.y * a.y * (1 - cosTheta);
	result.entry[1][2] = a.y * a.z * (1 - cosTheta) - a.x * sinTheta;

	result.entry[2][0] = a.z * a.x * (1 - cosTheta) - a.y * sinTheta;
	result.entry[2][1] = a.z * a.y * (1 - cosTheta) + a.x * sinTheta;
	result.entry[2][2] = cosTheta + a.z * a.z * (1 - cosTheta);

	return result;
}

RE::NiPointer<RE::NiCamera> GetNiCamera(RE::PlayerCamera* camera)
{
	// Do other things parent stuff to the camera node? Better safe than sorry I guess
	if (camera->cameraRoot->GetChildren().size() == 0)
		return nullptr;
	for (auto& entry : camera->cameraRoot->GetChildren()) {
		auto asCamera = skyrim_cast<RE::NiCamera*>(entry.get());
		if (asCamera)
			return RE::NiPointer<RE::NiCamera>(asCamera);
	}
	return nullptr;
}

void UpdateInternalWorldToScreenMatrix(RE::NiCamera* a_niCamera)
{
	using func_t = decltype(&UpdateInternalWorldToScreenMatrix);
	REL::Relocation<func_t> func{ REL::RelocationID(69271, 70641) };
	func(a_niCamera);
}

void UpdateCamera(RE::TESCamera* a_camera)
{
	RE::NiPoint3 rotationsVertical[] = {
		{ 0.0f, 0.0f, 0.0f },
		{ -1.0f, 0.0f, 0.0f },
		{ -1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f }
	};

	float thetasVertical[] = {
		0.0f,
		std::numbers::pi_v<float> * (1.0f / 4.0f),
		std::numbers::pi_v<float> * (2.0f / 4.0f),
		std::numbers::pi_v<float> * (1.0f / 4.0f),
		std::numbers::pi_v<float> * (2.0f / 4.0f),
	};

	RE::NiPoint3 rotationsHorizontal[] = {
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f }
	};

	float thetasHorizontal[] = {
		0.0f,
		std::numbers::pi_v<float> * (1.0f / 5.0f) * 2.0f,
		std::numbers::pi_v<float> * (2.0f / 5.0f) * 2.0f,
		std::numbers::pi_v<float> * (3.0f / 5.0f) * 2.0f,
		std::numbers::pi_v<float> * (4.0f / 5.0f) * 2.0f,
	};

	if (rotateCamera) {
		a_camera->cameraRoot->local.rotate = a_camera->cameraRoot->local.rotate * MatrixFromAxisAngle(rotationsVertical[(int)floor((float)captureStep / 5.0f)], thetasVertical[(int)floor((float)captureStep / 5.0f)]) * MatrixFromAxisAngle(rotationsHorizontal[captureStep % 5], thetasHorizontal[captureStep % 5]);

		RE::NiUpdateData updateData;
		a_camera->cameraRoot->UpdateDownwardPass(updateData, 0);

		auto playerCamera = RE::PlayerCamera::GetSingleton();
		auto niCamera = GetNiCamera(playerCamera);
		if (niCamera && niCamera.get())
			UpdateInternalWorldToScreenMatrix(niCamera.get());

		captureStep++;

		if (captureStep == ARRAYSIZE(rotationsVertical) * ARRAYSIZE(rotationsHorizontal)) {
			captureStep = 1;
			rotateCamera = false;
		}
	}
}

struct Hooks
{
	struct BSImagespaceShaderISSAOCompositeSAO_SetupTechnique
	{
		static void thunk(RE::BSShader* a_shader, RE::BSShaderMaterial* a_material)
		{
			DrawDeferred();
			func(a_shader, a_material);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	struct BSImagespaceShaderISSAOCompositeFog_SetupTechnique
	{
		static void thunk(RE::BSShader* a_shader, RE::BSShaderMaterial* a_material)
		{
			DrawDeferred();
			func(a_shader, a_material);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	struct BSImagespaceShaderISSAOCompositeSAOFog_SetupTechnique
	{
		static void thunk(RE::BSShader* a_shader, RE::BSShaderMaterial* a_material)
		{
			DrawDeferred();
			func(a_shader, a_material);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	struct TESCamera_Update
	{
		static void thunk(RE::TESCamera* a_camera)
		{
			func(a_camera);
			UpdateCamera(a_camera);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	static void Install()
	{
		logger::info("Hooking deferred passes");
		stl::write_vfunc<0x2, BSImagespaceShaderISSAOCompositeSAO_SetupTechnique>(RE::VTABLE_BSImagespaceShaderISSAOCompositeSAO[0]);
		stl::write_vfunc<0x2, BSImagespaceShaderISSAOCompositeFog_SetupTechnique>(RE::VTABLE_BSImagespaceShaderISSAOCompositeFog[0]);
		stl::write_vfunc<0x2, BSImagespaceShaderISSAOCompositeSAOFog_SetupTechnique>(RE::VTABLE_BSImagespaceShaderISSAOCompositeSAOFog[0]);
		
		logger::info("Hooking camera updates");
		stl::write_thunk_call<TESCamera_Update>(REL::RelocationID(49852, 50784).address() + REL::Relocate(0x1A6, 0x1A6, 0x1B6));
	}
};

class MenuOpenCloseEventHandler : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
{
public:
	virtual RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	{
		// When entering a new cell, reset the capture
		if (a_event->menuName == RE::LoadingMenu::MENU_NAME) {
			if (!a_event->opening)
				resetCapture = true;
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	static bool Register()
	{
		static MenuOpenCloseEventHandler singleton;
		auto ui = RE::UI::GetSingleton();

		if (!ui) {
			logger::error("UI event source not found");
			return false;
		}

		ui->GetEventSource<RE::MenuOpenCloseEvent>()->AddEventSink(&singleton);

		logger::info("Registered {}", typeid(singleton).name());

		return true;
	}
};

bool Load()
{
	Hooks::Install();
	MenuOpenCloseEventHandler::Register();
	return true;
}