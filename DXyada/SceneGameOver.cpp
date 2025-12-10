#include"SceneGameOver.h"
#include"input.h"
#include"SceneID.h"

extern Input input;

void SceneGameOver::Init() {

	g_cameraPos = { 0, 0, 0 };

	m_select.Init("asset/back.png");
	m_select.SetPos(0, 0, 0);
	m_select.SetSize(1920, 1080, 0);
}

void SceneGameOver::Update(SceneManager& mgr) {
	input.Update();

	if (input.GetKeyPress(VK_RETURN)) {
		mgr.ChangeScene(SCENE_GAME);
	}
	if (input.GetKeyPress(VK_SPACE)) {
		mgr.ChangeScene(SCENE_SELECT);
	}
}

void SceneGameOver::Draw() {
	m_select.Draw(
		g_pDeviceContext,
		g_pInputLayout,
		g_pVertexShader,
		g_pPixelShader,
		g_pConstantBuffer
	);
}

void SceneGameOver::UnInit() {
	m_select.UnInit();
}