#ifndef CHANEL_H
#define CHANEL_H

#include "system_message.h" // даёт message/user_message и ChanelInfo

// Тип канала
enum class ChanelType : quint8 {
    Text = 0,  // текстовый канал: история сообщений, файлы
    Voice = 1  // голосовой канал: релей аудио между участниками
};

// Канал сервера.
// Текстовый канал хранит историю сообщений (user_message, включая файлы),
// голосовой канал хранит только параметры — участники и релей управляются ChatServer.
class chanel
{
public:
    // Максимальный размер истории текстового канала (сообщений)
    static constexpr int MaxHistorySize = 500;

    chanel() = default;
    chanel(const QString& name, quint64 id, ChanelType type);

    // Добавить сообщение в историю (только для текстовых каналов).
    // При переполнении самые старые сообщения удаляются.
    void add_message(const user_message& m);

    // Краткое описание канала для ChanelsInfo
    ChanelInfo info() const;

    quint64 id = 0;
    QString name;
    ChanelType type = ChanelType::Text;
    QList<user_message> messages; // история (используется текстовыми каналами)
};

#endif // CHANEL_H
