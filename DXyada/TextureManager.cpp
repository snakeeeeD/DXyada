#include "TextureManager.h"
#include "Texture.h"

ID3D11ShaderResourceView* TextureManager::GetSRV(ID3D11Device* device, const char* filename)
{
    if (!device || !filename) return nullptr;

    std::string key = filename;

    auto it = m_cache.find(key);
    if (it != m_cache.end())
    {
        it->second->AddRef(); // 呼び出し側が保持する分
        return it->second;
    }

    // 初回だけ実ロード
    Texture tex;
    if (!tex.Load(device, filename))
    {
        return nullptr;
    }

    ID3D11ShaderResourceView* srv = tex.GetSRV(); // GetSRV() がある前提
    if (!srv) return nullptr;

    // キャッシュ保持分
    srv->AddRef();
    m_cache[key] = srv;

    // 呼び出し側へ渡す分
    srv->AddRef();
    return srv;
}

void TextureManager::Clear()
{
    // キャッシュ解放
    for (auto& kv : m_cache)
    {
        if (kv.second) kv.second->Release();
    }
    m_cache.clear();

    // キューもクリア
    while (!m_queue.empty()) m_queue.pop();
}

void TextureManager::Enqueue(const char* filename)
{
    if (!filename) return;

    std::string key = filename;

    // すでにキャッシュ済みならキュー不要
    if (m_cache.find(key) != m_cache.end())
        return;

    // すでにキューにあるかは本来チェックしたいが、queueは走査が重いので
    // まずは「重複Enqueueしない運用」でOK（AssetListはユニーク推奨）
    m_queue.push(key);
}

bool TextureManager::ProcessQueue(ID3D11Device* device, int maxCount)
{
    if (!device) return false;
    if (maxCount <= 0) maxCount = 1;

    for (int i = 0; i < maxCount; ++i)
    {
        if (m_queue.empty()) break;

        std::string path = m_queue.front();
        m_queue.pop();

        // 途中で別経路でロード済みになっている可能性がある
        if (m_cache.find(path) != m_cache.end())
            continue;

        ID3D11ShaderResourceView* srv = GetSRV(device, path.c_str());
        if (srv) srv->Release(); // 呼び出し側の分だけ解放（キャッシュは保持）
    }

    return m_queue.empty();
}
