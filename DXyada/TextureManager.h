#pragma once
#include <unordered_map>
#include <string>
#include <queue>
#include <d3d11.h>

class TextureManager
{
public:
    static TextureManager& Instance()
    {
        static TextureManager inst;
        return inst;
    }

    // 取得したSRVは AddRef された状態で返す（呼び出し側が Release）
    ID3D11ShaderResourceView* GetSRV(ID3D11Device* device, const char* filename);

    // 終了時に呼ぶ
    void Clear();

    // ===== 追加：事前予約（キュー）=====
    void Enqueue(const char* filename);

    // ===== 追加：ロード処理（少しずつ）=====
    // 1回呼ぶごとに「最大 maxCount 枚」だけロードする
    // 戻り値：キューが空になったら true
    bool ProcessQueue(ID3D11Device* device, int maxCount);

    bool IsQueueEmpty() const { return m_queue.empty(); }
    int  GetQueueSize() const { return (int)m_queue.size(); }

private:
    TextureManager() {}
    TextureManager(const TextureManager&);
    TextureManager& operator=(const TextureManager&);

private:
    std::unordered_map<std::string, ID3D11ShaderResourceView*> m_cache;

    // 追加：ロード予約キュー
    std::queue<std::string> m_queue;
};
