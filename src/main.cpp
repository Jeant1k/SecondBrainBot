#include <iostream>
#include <vector>
#include <random>

#include <tgbot/tgbot.h>
#include "DatabaseService/include/DatabaseService.hpp"

// TODO: Функционал "сделай это прямо сейчас"

const std::vector<std::string> lists = {
    "Заметки",
    "Ожидание",
    "Календарь",
    "Когда-нибудь потом",
    "Проекты",
    "Текущие действия"
};

const std::vector<std::string> notes_categories = {
    "Работа",
    "Личные",
    "Здоровье",
    "Покупки",
    "Финансы",
    "Книги/Фильмы/Музыка",
    "Туризм",
    "Еда",
    "Учеба",
    "Проекты"
};

std::vector<std::string> someday_categories = {
    "Обучение и развитие",
    "Путешествия",
    "Здоровье и фитнес",
    "Личные проекты",
    "Карьера и работа",
    "Деньги",
    "Социальные и семейные отношения",
    "Духовность",
    "Волонтерство и благотворительность",
    "Личное благополучие"
};

std::unordered_map<int64_t, std::string> chatStates;


int main() {

    TgBot::Bot bot("5699724308:AAEO8PxBfWj-Bnd2Y6WYRyT2NJPWdjrqkLY");

    Database db("dbname=secondbrainbot user=jeantik password=123 hostaddr=127.0.0.1 port=5432");
    db.connect();


    // Объявление команд

    std::vector<std::string> bot_commands = {"start", "help", "support"};

    std::vector<TgBot::BotCommand::Ptr> commands;

    TgBot::BotCommand::Ptr cmdArray(new TgBot::BotCommand);
    cmdArray->command = "start";
    cmdArray->description = "Запустить бота";
    commands.push_back(cmdArray);

    cmdArray = TgBot::BotCommand::Ptr(new TgBot::BotCommand);
    cmdArray->command = "help";
    cmdArray->description = "Справка";
    commands.push_back(cmdArray);

    cmdArray = TgBot::BotCommand::Ptr(new TgBot::BotCommand);
    cmdArray->command = "support";
    cmdArray->description = "Поддержка";
    commands.push_back(cmdArray);

    bot.getApi().setMyCommands(commands);

    std::vector<TgBot::BotCommand::Ptr> vectCmd;
    vectCmd = bot.getApi().getMyCommands();

    for(std::vector<TgBot::BotCommand::Ptr>::iterator it = vectCmd.begin(); it != vectCmd.end(); ++it)
        std::cout << "Команды: " << (*it)->command << " -> " << (*it)->description << std::endl;


    // Основа

    // Зачем в БД хранить списки для каждого пользователя? Они же одинаковые. Можно просто использовать ID этих списков для всех.
    bot.getEvents().onCommand("start", [&bot, &db](TgBot::Message::Ptr message) {
        bot.getApi().sendChatAction(message->chat->id, "typing");
        auto result = db.call_procedure("register_user", message->chat->id, message->from->firstName);
        if (result && *result)
            bot.getApi().sendMessage(message->chat->id, "*Пришли мне любую задачу или заметку!*\n\nДля того, чтобы узнать как я работаю, нажми /help.", false, 0, nullptr, "Markdown");
        else
            bot.getApi().sendMessage(message->chat->id, "Привет!  🙌\nЯ умный бот, который поможет автоматизировать ведение твоей системы GTD.\n\n*Пришли мне любую задачу или заметку!*\n\nДля того, чтобы узнать как я работаю, нажми /help.", false, 0, nullptr, "Markdown");
    });

    bot.getEvents().onCommand("help", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendChatAction(message->chat->id, "typing");
        bot.getApi().sendMessage(message->chat->id, "Подробная справка должна быть здесь. Сделать меню в виде кнопок с инструкцией к боту. Тут должно быть обучение, описание функций бота, описание того как с ним взаимодействовать, теоретические сведения о системе ГТД, советы по использованию и т.д.");
    });

    bot.getEvents().onCommand("support", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendChatAction(message->chat->id, "typing");
        bot.getApi().sendMessage(message->chat->id, "Вам что-то не понравилось? Что-то не работает? У вас есть пожелания или предложения?\n\nПишите @Jeant1k, будем очень рады вашему отзыву!");
    });

    bot.getEvents().onAnyMessage([&bot, &bot_commands, &db](TgBot::Message::Ptr message) {
        bot.getApi().sendChatAction(message->chat->id, "typing");

        std::cout << "Новое сообщение: " << message->text << std::endl;
        for (const auto& command : bot_commands)
            if ("/" + command == message->text) return;

        if (chatStates[message->chat->id] == "waitingForDateTime") {
            chatStates[message->chat->id] = "";
            std::ifstream fileStream("/home/jeantik/programs/SecondBrainBot/files/template.ics", std::ios::binary);
            std::string fileContent((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
            TgBot::InputFile::Ptr inputFile(new TgBot::InputFile);
            inputFile->fileName = "template.ics";
            inputFile->mimeType = "text/calendar";
            inputFile->data = fileContent;
            bot.getApi().sendMessage(message->chat->id, "Вот событие для вашего календаря");
            bot.getApi().sendDocument(message->chat->id, inputFile);
        } else {
            std::random_device rd;
            std::mt19937 gen(rd());

            std::uniform_int_distribution<> distrib(0, lists.size() - 1);
            std::string list = lists[distrib(gen)];

            if (list == "Заметки") {
                std::uniform_int_distribution<> distrib(0, notes_categories.size() - 1);
                std::string categorie = notes_categories[distrib(gen)];
                std::string description = "description";
                db.call_procedure("add_task", list, message->text, description, categorie);
                bot.getApi().sendMessage(message->chat->id, "Добавил в список " + list + " в категорию " + categorie);
            } else if (list == "Ожидание") {
                db.call_procedure("add_task", list, message->text);
                bot.getApi().sendMessage(message->chat->id, "Добавил в список " + list);
                bot.getApi().sendMessage(message->chat->id, "Здесь будет индивидуальный шаблон для делегирования задачи человеку.");
            } else if (list == "Календарь") {
                std::uniform_int_distribution<> distrib(0, 1);
                if (distrib(gen)) {
                    bot.getApi().sendMessage(message->chat->id, "Уточните дату и время");
                    chatStates[message->chat->id] = "waitingForDateTime";
                } else {
                    std::ifstream fileStream("/home/jeantik/programs/SecondBrainBot/files/template.ics", std::ios::binary);
                    std::string fileContent((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
                    TgBot::InputFile::Ptr inputFile(new TgBot::InputFile);
                    inputFile->fileName = "template.ics";
                    inputFile->mimeType = "text/calendar";
                    inputFile->data = fileContent;
                    bot.getApi().sendMessage(message->chat->id, "Вот событие для вашего календаря");
                    bot.getApi().sendDocument(message->chat->id, inputFile);
                }
            } else if (list == "Когда-нибудь потом") {
                std::uniform_int_distribution<> distrib(0, someday_categories.size() - 1);
                std::string categorie = someday_categories[distrib(gen)];
                std::string description = "description";
                db.call_procedure("add_task", list, message->text, description, categorie);
                bot.getApi().sendMessage(message->chat->id, "Добавил в список " + list + " в категорию " + categorie);
            } else if (list == "Проекты") {
                std::string description = "description";
                db.call_procedure("add_task", list, message->text, description);
                bot.getApi().sendMessage(message->chat->id, "Здесь будет пример расписанного проекта: критерий завершения, план, первый шаг");
            } else if (list == "Текущие действия") {
                std::uniform_int_distribution<> distrib(0, 1);
                if (distrib(gen)) {
                    bot.getApi().sendMessage(message->chat->id, "Сделай это прямо сейчас!");
                } else {
                    db.call_procedure("add_task", list, message->text);
                    bot.getApi().sendMessage(message->chat->id, "Добавил в список " + list);
                }
            } else if (list == "Архив") {
                // db.call_procedure("archive");
                bot.getApi().sendMessage(message->chat->id, "Переместил в архив");
            } else if (list == "Сделано") {
                // db.call_procedure("move_to_done");
                bot.getApi().sendMessage(message->chat->id, "Переместил в сделано");
            } else {
                bot.getApi().sendMessage(message->chat->id, "Категория не распознана");
            }
        }
    });

    signal(SIGINT, [](int s) {
        std::cout << std::endl << "SIGINT поймал" << std::endl;
        exit(0);
    });

    try {
        std::cout << "Имя бота: " << bot.getApi().getMe()->username << std::endl;
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            std::cout << "LongPoll запустился" << std::endl;
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        std::cout << "Ошибка: " << e.what() << std::endl;
    }

    return 0;

}