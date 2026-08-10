#include "Language.h"

#include "..\Util\ExePath.h"
#include "..\Util\FileLogger.h"
#include "..\Menu\Menu.h"

#include <fstream>
#include <string>
#include <cstring>
#include <Windows.h>
#include <algorithm>
#include <json\single_include\nlohmann\json.hpp>
#include <set>
using Json = nlohmann::json;

namespace Language
{
	std::vector<Lang> allLangs;
	Lang* selectedLang = nullptr;
	std::string configLangName = "English";

	// Must be compiled with /utf-8. Stored as JSON so runtime load + file export share one source.
	static const char* BuiltinRussianJson()
	{
		// Split to avoid MSVC C2026 (string literal too long).
		static std::string s;
		if (s.empty())
		{
			s = std::string(R"json({
  "MAIN": "ГЛАВНАЯ",
  "Players": "Игроки",
  "Player Options": "Игрок",
  "Player": "Игрок",
  "Weapons": "Оружие",
  "Vehicles": "Транспорт",
  "Teleport": "Телепорт",
  "Spawner": "Спавнер",
  "World": "Мир",
  "Weather": "Погода",
  "Time": "Время",
  "NPC": "NPC",
  "Functions": "Функции",
  "Chaos Modes": "Безумные режимы",
  "Mad Runs": "Безумные забеги",
  "Jet Stunt Mode": "Трюки на истребителе",
  "John Wick Mode": "Режим Джона Уика",
  "Alien Invasion": "Вторжение инопланетян",
  "Animal Chaos": "Хаос с животными",
  "Stop Active Mode": "Остановить режим",
  "FreeCam (No-Clip)": "Свободная камера",
  "Super Run": "Супер бег",
  "Super Jump": "Супер прыжок",
  "God Mode": "Бессмертие",
  "No Ragdoll": "Без рэгдолла",
  "Never Wanted": "Без розыска",
  "Show Coordinates": "Показать координаты",
  "Show FPS": "Показать FPS",
  "Infinite Ammo": "Бесконечные патроны",
  "Explosive Melee": "Взрывной рукопашный",
  "Rapid Fire": "Быстрая стрельба",
  "Drive On Water": "Езда по воде",
  "Misc Options": "Разное",
  "Hero Abilities": "Способности героев",
  "Custom Skins & Heroes": "Скины и герои",
  "Creators / Spooner": "Создатели / Спунер",
  "Cutscene Player": "Плеер катсцен",
  "TV Player": "ТВ-плеер",
  "Radio": "Радио",
  "Animal Riding (SP)": "Езда на животных",
  "Spawn Bodyguard": "Нанять телохранителя",
  "Bodyguard List": "Список телохранителей",
  "No bodyguards spawned": "Нет телохранителей",
  "Hint: use Quick Spawn or Spawn Bodyguard": "Подсказка: быстрый спавн или нанять",
  "Quick Spawn": "Быстрый спавн",
  "SWAT": "SWAT",
  "Merryweather": "Merryweather",
  "Army": "Армия",
  "Police": "Полиция",
  "FIB": "FIB",
  "Gang Ballas": "Банда Ballas",
  "Marine": "Морпех",
  "Clone Player Model": "Клон игрока",
  "Appearance": "Внешний вид",
  "Statistics": "Статистика",
  "SPACE": "SPACE",
  "Misc": "Разное",
  "Character Stats": "Статистика персонажа",
  "Settings": "Настройки",
  "Menu": "Меню",
  "Bodyguards": "Телохранители",
  "Creators": "Создатели",
  "Animations": "Анимации",
  "All Ped Animations": "Все анимации педов",
  "Scenarios": "Сценарии",
  "Movement Styles": "Стили ходьбы",
  "Facial Moods": "Настроение лица",
  "Spawn Bodyguard": "Нанять телохранителя",
  "Bodyguard List": "Список телохранителей",
  "No bodyguards spawned": "Нет телохранителей",
  "Hint: use Quick Spawn or Spawn Bodyguard": "Подсказка: быстрый спавн или нанять",
  "Default Health": "Здоровье по умолчанию",
  "Default Armor": "Броня по умолчанию",
  "Godmode": "Бессмертие",
  "Bodyguard Blip": "Блип телохранителя",
  "Formation": "Формация",
  "Aggressive Combat": "Агрессивный бой",
  "Give All Weapons": "Выдать всё оружие",
  "Heal All Bodyguards": "Вылечить всех",
  "Put Bodyguards In My Vehicle": "Посадить в мой транспорт",
  "Bring Bodyguards To Self": "Телепорт к себе",
  "Cycle Control (Next Bodyguard) (SP)": "Переключить управление (SP)",
  "Dismiss All Bodyguards": "Уволить всех",
  "Quick Spawn": "Быстрый спавн",
  "---Quick Spawn---": "---Быстрый спавн---",
  "---Group Actions---": "---Действия группы---",
  "---Categories---": "---Категории---",
  "---Animations---": "---Анимации---",
  "SWAT": "SWAT",
  "Merryweather": "Merryweather",
  "Army": "Армия",
  "Police": "Полиция",
  "FIB": "FIB",
  "Gang Ballas": "Банда Ballas",
  "Marine": "Морпех",
  "Clone Player Model": "Клон модели игрока",
  "Player and NPC animations.": "Анимации игрока и NPC.",
  "FreeCam (No-Clip)": "Свободная камера",
  "Super Run": "Супербег",
  "Super Jump": "Суперпрыжок",
  "God Mode": "Режим бога",
  "No Ragdoll": "Без рэгдолла",
  "Never Wanted": "Без розыска",
  "Show Coordinates": "Показать координаты",
  "Infinite Ammo": "Бесконечные патроны",
  "Explosive Melee": "Взрывной рукопашный",
  "Rapid Fire": "Быстрая стрельба",
  "Drive On Water": "Езда по воде",
  "Decreased Ped Population": "Меньше пешеходов",
  "Decreased Vehicle Population": "Меньше транспорта",
  "Clear Weapon Pickups": "Убрать пикапы оружия",
  "EMP / Blackout": "EMP / затемнение",
  "Simple Blackout": "Простое затемнение",
  "Massacre Mode": "Режим бойни",
  "Restricted Area Access": "Доступ в запретные зоны",
  "Clear Area": "Очистить зону",
  "PC Graphics": "Графика ПК",
  "Space Extras": "Space Extras",
  "Creators": "Создатели",
  "Spawn Ped / Model": "Спавн педа / модели",
  "Companions / Clones": "Компаньоны / клоны",
  "Slow Motion": "Замедление времени",
  "Soft Landing (No Fall Damage)": "Мягкое приземление",
  "Freeze Nearby NPCs/Cars": "Заморозить рядом",
  "Vehicle Hover (Anti-Gravity)": "Парение транспорта",
  "Auto Clean Aura": "Автоочистка",
  "Night Vision": "Ночное зрение",
  "Thermal Vision": "Тепловизор",
  "Walk On Water": "Ходьба по воде",
  "Tiny Player": "Крошечный игрок",
  "One-Hit Kill": "Убийство с одного удара",
  "Cash Rain Around Me": "Дождь из денег",
  "Health Pack Drop": "Аптечки рядом",
  "Warp Forward 12m": "Рывок вперёд 12м",
  "Skydive From Height": "Парашют с высоты",
  "Party Godmode (You + Car)": "Godmode себе и машине",
  "Repair All Nearby Vehicles": "Починить весь транспорт рядом",
  "Clean Ped": "Очистить персонажа",
  "Suicide": "Самоубийство",
  "Invincibility": "Бессмертие",
  "Replenish Player": "Восстановить игрока",
  "Wanted Level": "Уровень розыска",
  "Freeze Wanted Level": "Заморозить розыск",
  "Ignored By Everyone": "Игнорируют все",
  "Fly Manual": "Полёт вручную",
  "Fly Auto": "Автополёт",
  "Custom Skins & Heroes": "Скины и герои",
  "Hero Abilities": "Способности героев",
  "Superman Punch": "Удар Супермена",
  "Peds / Models": "Педы / Модели",
  "SPACE UI": "Внешний вид",
  "UI Opacity %": "Прозрачность UI %",
  "UI Scale %": "Масштаб UI %",
  "Blur / Dim %": "Размытие %",
  "Animation": "Анимация",
  "Live Pulse": "Живой пульс",
  "Show Tips": "Подсказки",
  "Row Dividers": "Разделители",
  "Show FPS": "Показать FPS",
  "Theme": "Тема",
  "Transparency %": "Прозрачность %",
  "Menu Scale %": "Масштаб меню %",
  "Text Scale %": "Масштаб текста %",
  "Blur Intensity %": "Интенсивность blur %",
  "Reset Appearance": "Сбросить внешний вид",
  "Save As Custom Theme...": "Сохранить тему...",
  "Custom Themes Folder": "Папка тем",
  "Menu Position": "Позиция меню",
  "Menu Colours (Advanced)": "Цвета меню",
  "Language": "Язык",
  "Themes": "Темы",
  "Settings": "Настройки",
  "MAIN": "ГЛАВНАЯ",
  "Player Options": "Игрок",
  "Vehicle Options": "Транспорт",
  "Weapon Options": "Оружие",
  "Teleport Options": "Телепорт",
  "Misc Options": "Разное",
  "Weather Options": "Погода",
  "Time Options": "Время",
  "Object Spooner": "Спунер",
  "Bodyguard Options": "Телохранители",
  "SPACE UI": "Внешний вид",
  "Transparency %": "Прозрачность %",
  "Menu Scale %": "Масштаб меню %",
  "Text Scale %": "Масштаб текста %",
  "Blur Intensity %": "Интенсивность blur %",
  "Show Tips": "Показывать подсказки",
  "Row Dividers": "Разделители строк",
  "Show FPS": "Показывать FPS",
  "Reset Appearance": "Сбросить внешний вид",
  "Theme": "Тема",
  "Creator Profile": "Профиль создателя",
  "Website": "Сайт",
  "GitHub": "GitHub",
  "Character Stats Editor": "Редактор статистики",
  "Toggle FPS Overlay": "Переключить FPS",
  "Player abilities, health and movement.": "Способности, здоровье и движение.",
  "Hire and manage elite bodyguards.": "Наём и управление телохранителями.",
  "Weapons, ammo and combat modifiers.": "Оружие, патроны и бой.",
  "Current vehicle tools and godmode.": "Транспорт: инструменты и godmode.",
  "Spawn vehicles, peds, animals and props.": "Спавн транспорта, педов и объектов.",
  "Travel the map instantly.": "Мгновенные перемещения.",
  "Control the environment.": "Управление игровым окружением.",
  "Change and freeze weather.": "Смена и заморозка погоды.",
  "Set, freeze and speed up time.": "Установка и ускорение времени.",
  "Spawn and control NPCs.": "Спавн и управление NPC.",
  "Freecam, noclip and extras.": "Freecam, noclip и доп. функции.",
  "SPACE look, themes and UI scale.": "Внешний вид SPACE и темы.",
  "Live player and world statistics.": "Живая статистика.",
  "Language, binds and config.": "Язык, клавиши и конфиг.",
  "About SPACE trainer and links.": "О трейнере SPACE и ссылки.",
  "Live Pulse": "Живой пульс",
  "Control player abilities and character settings.": "Способности и настройки игрока.",
  "Manage weapons, ammunition and combat settings.": "Оружие, патроны и бой.",
  "Spawn, repair and tune vehicles.": "Спавн, ремонт и тюнинг.",
  "Travel across Los Santos instantly.": "Мгновенный телепорт.",
  "Object spooner: place, edit and save props.": "Спунер: объекты и сцены.",
  "Spawn and manage immortal smart bodyguards.": "Спавн и управление телохранителями.",
  "Spawn peds, vehicles, objects and companions.": "Спавн педов, машин и объектов.",
  "Spawn peds, vehicles, objects and bodyguards.": "Спавн педов, машин и объектов.",
  "Time, weather, traffic and world control.": "Время, погода и мир.",
  "Play animations and scenarios.": "Анимации и сценарии.",
  "Extra tools, FPS, skins and utilities.": "Доп. инструменты и утилиты.",
  "View and edit SP character statistics.": "Статистика персонажа SP.",
  "UI, language, themes and Space options.": "UI, язык и настройки.",
  "Mega Jump": "Мега-прыжок",
  "Infinite Stamina": "Бесконечная выносливость",
  "Quiet Steps": "Тихие шаги",
  "Force Field (Push NPCs)": "Силовое поле",
  "Pro Pack": "Pro Pack",
  "Cleanup, film cams, gangs, traffic.": "Очистка, камеры, банды, трафик.",
  "Freecam, chaos modes and extras.": "Freecam, хаос и доп. функции.",
  "Space Pro Pack": "Space Pro Pack",
  "Quick Search (~)": "Быстрый поиск (~)",
  "Smart Vehicle Cleanup": "Умная очистка машин",
  "Film Cameras": "Камеры для съёмки",
  "Car Tuning (Mod Shop)": "Тюнинг (Mod Shop)",
  "Custom Gangs": "Кастомные банды",
  "Realistic Traffic": "Реалистичный трафик",
  "Safe Mode (mod compatibility)": "Safe Mode (совместимость)",
  "Max Upgrade Current Car": "Макс. тюнинг текущей машины",
  "Repair + Clean Current Car": "Ремонт + мойка",
  "Vehicle Saver Folder": "Сохранённые машины",
  "Delete empty spawned": "Удалить пустые созданные",
  "Delete all spawned (keep current)": "Удалить все созданные (оставить текущую)",
  "Enable Realistic Traffic": "Включить реалистичный трафик",
  "Orbit Cam": "Орбитальная камера",
  "Chase Cam": "Камера погони",
  "Save Nearby Peds As Gang": "Сохранить педов рядом как банду",
  "Stop Film Camera": "Выключить film-камеру",
  "Contract Hits": "Заказные убийства",
  "Gang Assault": "Нападение банд",
  "Solo Contract": "Одиночное задание",
  "Mobile Hit": "Выездное убийство",
  "Boat Hit": "Выезд на лодке",
  "Sniper Contract": "Снайперский контракт",
  "Abort Contract": "Отменить контракт",
  "Auto Flip Car": "Автопереворот авто",)json") + std::string(R"json(
  "Clear Wanted": "Сбросить розыск",
  "Max Wanted (5 Stars)": "Розыск 5 звёзд",
  "Random Outfit": "Случайный наряд",
  "Object Gun": "Пушка объектов",
  "Titlebox Globe": "Блик заголовка",
  "Peds / Models": "Педы / Модели",
  "Animals": "Животные",
  "Companions / Clones": "Компаньоны / Клоны",
  "Accent Color": "Цвет акцента",
  "UI Opacity %": "Прозрачность UI %",
  "UI Scale %": "Масштаб UI %",
  "Blur / Dim %": "Размытие / затемнение %",
  "Animation": "Анимация",
  "Reset SPACE UI": "Сброс SPACE UI",
  "Vehicle Options": "Транспорт",
  "Teleport Options": "Телепорт",
  "Weapon Options": "Оружие",
  "Weather Options": "Погода",
  "Time Options": "Время",
  "Bodyguard Options": "Телохранители",
  "Object Spooner": "Спунер",
  "Misc Options": "Разное",
  "Creator": "Создатель",
  "Creator": "Создатель",
  "Language": "Язык",
  "Themes": "Темы",
  "Custom Themes (Shareable)": "Кастомные темы (обмен)",
  "Custom Themes": "Кастомные темы",
  "Reload Themes Folder": "Обновить папку тем",
  "Save Current Theme As...": "Сохранить текущую тему как...",
  "Save As Custom Theme...": "Сохранить как кастомную тему...",
  "Custom Themes Folder": "Папка кастомных тем",
  "Delete Selected Theme File": "Удалить выбранный файл темы",
  "Drop .ini files here to share/use": "Кидайте .ini сюда для обмена/активации",
  "No custom themes yet": "Пока нет кастомных тем",
  "SPACE UI Settings": "Настройки SPACE UI",
  "Menu Colours": "Цвета меню",
  "Menu Fonts": "Шрифты меню",
  "Menu Position": "Позиция меню",
  "Mouse Support": "Мышь",
  "Gradients": "Градиенты",
  "Titlebox Globe": "Глобус",
  "Centre Title": "Заголовок по центру",
  "Centre Options": "Опции по центру",
  "Centre Breaks": "Разделители по центру",
  "Reset Player Model Upon Death (SP)": "Сброс модели после смерти",
  "Sync Space With Config File": "Синхронизация с конфигом",
  "Sync Space With Config File": "Синхронизация с конфигом",
  "Reset Toggles (Most Of Them)": "Сброс переключателей",
  "English": "English",
  "Russian": "Russian",
  "Reload Language Files": "Перезагрузить языки",
  "Offset X": "Смещение X",
  "Offset Y": "Смещение Y",
  "Vehicles": "Транспорт",
  "Vehicle Spawner": "Спавнер",
  "Spawn Settings": "Настройки спавна",
  "Favourites": "Избранное",
  "Saved Vehicles": "Сохранённый транспорт",
  "Delete Vehicle": "Удалить транспорт",
  "Repair Vehicle": "Починить транспорт",
  "Auto-Repair": "Авто-починка",
  "Seatbelt": "Ремень",
  "Invincibility": "Бессмертие",
  "Never Wanted": "Без розыска",
  "Give All Weapons": "Выдать всё оружие",
  "Infinite Ammo In Clip": "Бесконечные патроны",
  "Explosive Melee": "Взрывной рукопашный",
  "Superman Punch": "Удар Супермена",
  "Waypoint": "Метка",
  "Teleport Into Closest Vehicle": "В ближайший транспорт",
  "Creator: xdigr": "Создатель: xdigr",
  "Telegram: t.me/xdigr": "Telegram: t.me/xdigr",
  "Online/local players list and actions.": "Список игроков и действия.",
  "Godmode, powers, movement, wanted level.": "Бессмертие, силы, движение, розыск.",
  "Current vehicle options and spawner.": "Опции транспорта и спавнер.",
  "Teleport to map places and coordinates.": "Телепорт по карте и координатам.",
  "Weapons, ammo and special guns.": "Оружие, патроны и особое оружие.",
  "Change weather and world atmosphere.": "Смена погоды и атмосферы.",
  "Change clock time and freeze time.": "Смена времени и заморозка часов.",
  "Spawn and manage bodyguards.": "Спавн и управление телохранителями.",
  "Place, edit and save world entities.": "Размещение и сохранение объектов.",
  "Extra world and fun options.": "Дополнительные опции.",
  "Language, look, binds and config.": "Язык, вид, бинды и конфиг.",
  "Creator info, version and Telegram.": "Инфо о создателе и Telegram.",
  "Space trainer creator.": "Создатель трейнера Space.",
  "Open creator Telegram channel.": "Открыть Telegram создателя.",
  "Switch menu language (English / Russian).": "Сменить язык меню (EN / RU).",
  "Load saved colour themes.": "Загрузить темы цветов.",
  "Customize menu colours.": "Настроить цвета меню.",
  "Customize menu fonts.": "Настроить шрифты меню.",
  "Move the menu on screen.": "Переместить меню на экране.",
  "Enable mouse navigation in the menu.": "Навигация мышью.",
  "Browse and spawn vehicles.": "Просмотр и спавн транспорта.",
  "Auto-sit, upgrades and spawn flags.": "Автопосадка и флаги спавна.",
  "Your favourite vehicle models.": "Избранные модели.",
  "Load vehicles saved as XML.": "Загрузка транспорта из XML.",
  "Punch/kick knocks people and cars away.": "Удар отбрасывает людей и машины.",
  "Player cannot take damage.": "Игрок не получает урон.",
  "Police wanted level stays at zero.": "Розыск всегда ноль.",
  "Gives every weapon with ammo.": "Выдаёт всё оружие.",
  "Open vehicle categories and spawn cars.": "Категории и спавн транспорта.",
  "Deletes the current vehicle.": "Удаляет текущий транспорт.",
  "Fully repairs the current vehicle.": "Чинит текущий транспорт.",
  "Keeps repairing the vehicle automatically.": "Автоматическая починка.",
  "Prevents being ejected from the vehicle.": "Не выбрасывает из транспорта.",
  "Melee hits cause explosions.": "Рукопашные удары взрывают.",
  "Magazine never empties.": "Магазин не опустошается.",
  "Warps you into nearest free vehicle.": "В ближайший свободный транспорт.",
  "Teleport to your map waypoint.": "Телепорт к метке.",
  "Use English menu text.": "Английский текст меню.",
  "Use Russian menu text.": "Русский текст меню.",
  "Reload .json language files from disk.": "Перезагрузить языки с диска.",
  "Safe to spawn.": "Безопасно спавнить.",
  "Crash risk: missing/unsupported model. Prefer another car.": "Риск вылета: модель не поддерживается.",
  "Customize the Trainer.": "Настройки трейнера.",
  "Bodyguards": "Телохранители",
  "Spawn Bodyguard": "Создать телохранителя",
  "Bodyguard List": "Список телохранителей",
  "Default Health": "Здоровье",
  "Default Armor": "Броня",
  "Godmode": "Бессмертие",
  "Bodyguard Blip": "Блип телохранителя",
  "Formation": "Формация",
  "Model Changer": "Смена скина",
  "Randomize Ped Variation": "Случайный внешний вид",
  "Random": "Случайный",
  "Player": "Игрок",
  "Animals": "Животные",
  "Ambient Females": "Женщины (ambient)",
  "Ambient Males": "Мужчины (ambient)",
  "Cutscene Models": "Катсцены",
  "Gang Females": "Банды (жен)",
  "Gang Males": "Банды (муж)",
  "Story Models": "Сюжетные",
  "Multiplayer Models": "Мультиплеер",
  "Scenario Females": "Сценарии (жен)",
  "Scenario Males": "Сценарии (муж)",
  "Story Scenario Females": "Сюжетные сценарии (жен)",
  "Story Scenario Males": "Сюжетные сценарии (муж)",
  "Others": "Другие",
  "Favourites": "Избранное",
  "Powers": "Способности",
  "Superman Punch": "Удар Супермена",
  "Wanted Level": "Уровень розыска",
  "Clear Wanted Level": "Сбросить розыск",
  "Invisible": "Невидимость",
  "No Ragdoll": "Без рэгдолла",
  "Fast Run": "Быстрый бег",
  "Super Jump": "Суперпрыжок",
  "Weapon Options": "Оружие",
  "Weather Options": "Погода",
  "Time Options": "Время",
  "Misc Options": "Разное",
  "Teleport Options": "Телепорт",
  "Component Changer": "Одежда",
  "Outfits": "Образы",
  "Accessories": "Аксессуары",
  "Heal": "Вылечить",
  "Delete All Bodyguards": "Удалить всех",
  "Teleport Bodyguards To Me": "Телепорт телохранителей ко мне",
  "Aggressive Combat": "Агрессивный бой",
  "Standard": "Стандарт",
  "Friend": "Друг",
  "VIP": "VIP",
  "Default Formation": "По умолчанию",
  "Circle (Inward)": "Круг (внутрь)",
  "Circle (North)": "Круг (север)",
  "Line": "Линия",
  "---Categories---": "---Категории---",
  "Space Pro Pack": "Space Pro Pack",
  "Quick Search (~)": "Быстрый поиск (~)",
  "Smart Vehicle Cleanup": "Умная очистка машин",
  "Film Cameras": "Камеры для съёмки",
  "Car Tuning (Mod Shop)": "Тюнинг (Mod Shop)",
  "Custom Gangs": "Кастомные банды",
  "Realistic Traffic": "Реалистичный трафик",
  "Safe Mode (mod compatibility)": "Safe Mode (совместимость)",
  "Max Upgrade Current Car": "Макс. тюнинг текущей машины",
  "Repair + Clean Current Car": "Ремонт + мойка",
  "Vehicle Saver Folder": "Сохранённые машины",
  "Delete empty spawned": "Удалить пустые созданные",
  "Delete all spawned (keep current)": "Удалить все созданные (оставить текущую)",
  "Enable Realistic Traffic": "Включить реалистичный трафик",
  "Orbit Cam": "Орбитальная камера",
  "Chase Cam": "Камера погони",
  "Save Nearby Peds As Gang": "Сохранить педов рядом как банду",
  "Stop Film Camera": "Выключить film-камеру",
  "Contract Hits": "Заказные убийства",
  "Gang Assault": "Нападение банд",
  "Solo Contract": "Одиночное задание",
  "Mobile Hit": "Выездное убийство",
  "Boat Hit": "Выезд на лодке",
  "Sniper Contract": "Снайперский контракт",
  "Abort Contract": "Отменить контракт"
})json");
		}
		return s.c_str();
	}

	static std::map<std::string, std::string> ParseBuiltinRussian()
	{
		std::map<std::string, std::string> dict;
		try
		{
			Json doc = Json::parse(BuiltinRussianJson());
			for (auto it = doc.begin(); it != doc.end(); ++it)
			{
				if (it.value().is_string())
					dict[it.key()] = it.value().get<std::string>();
			}
		}
		catch (...)
		{
			addlog(ige::LogType::LOG_ERROR, "Failed to parse builtin Russian JSON");
		}
		return dict;
	}

	static void EnsureLanguageFolderAndRussianFile()
	{
		const std::string mainDir = GetPathffA(Pathff::Main, false);
		const std::string langDir = GetPathffA(Pathff::Language, false);
		CreateDirectoryA(mainDir.c_str(), nullptr);
		CreateDirectoryA(langDir.c_str(), nullptr);

		const std::string ruPath = langDir + "\\Russian.json";
		// Never clobber a shipped/full translation file — only create when missing.
		if (does_file_exist(ruPath))
			return;

		std::ofstream out(ruPath, std::ios::binary | std::ios::trunc);
		if (out.is_open())
		{
			const char* json = BuiltinRussianJson();
			out.write(json, static_cast<std::streamsize>(strlen(json)));
			addlog(ige::LogType::LOG_INFO, "Created default Russian language file: " + ruPath);
		}
	}

	Lang::Lang(std::string aName, std::string aFilePath)
	{
		this->filepath = aFilePath;
		this->name = aName;
		this->pairs.clear();
	}

	std::string Lang::Translate(std::string text)
	{
		static std::set<std::string> reported_missing;

		auto it = this->pairs.find(text);
		if (it != this->pairs.end()) {
			return it->second;
		}

		// Retry without GTA colour/formatting codes (~r~, ~bold~, etc.)
		std::string stripped;
		stripped.reserve(text.size());
		for (size_t i = 0; i < text.size(); )
		{
			if (text[i] == '~')
			{
				size_t j = text.find('~', i + 1);
				if (j != std::string::npos)
				{
					i = j + 1;
					continue;
				}
			}
			stripped.push_back(text[i++]);
		}
		while (!stripped.empty() && stripped.front() == ' ') stripped.erase(stripped.begin());
		while (!stripped.empty() && stripped.back() == ' ') stripped.pop_back();

		if (!stripped.empty() && stripped != text)
		{
			auto it2 = this->pairs.find(stripped);
			if (it2 != this->pairs.end())
			{
				// Keep original colour codes around the translated core when possible.
				std::string prefix, suffix;
				if (!text.empty() && text.front() == '~')
				{
					size_t end = text.find('~', 1);
					while (end != std::string::npos && end + 1 < text.size() && text[end + 1] == '~')
					{
						// consecutive codes like ~r~~bold~
						size_t next = text.find('~', end + 2);
						if (next == std::string::npos) break;
						end = next;
					}
					if (end != std::string::npos)
						prefix = text.substr(0, end + 1);
				}
				if (text.size() > 2 && text.back() == '~')
				{
					size_t start = text.rfind('~', text.size() - 2);
					if (start != std::string::npos)
						suffix = text.substr(start);
				}
				const std::string result = prefix + it2->second + suffix;
				this->pairs[text] = result;
				return result;
			}
		}

		if (reported_missing.insert(text).second)
		{
			addlog(ige::LogType::LOG_ERROR, "Missing translation for: " + text);
		}
		this->pairs[text] = text;
		return text;
	}

	std::string TranslateToSelected(std::string text)
	{
		if (selectedLang != nullptr)
			return selectedLang->Translate(text);
		else
		{
			return text;
		}
	}

	int Init()
	{
		allLangs.clear();
		selectedLang = nullptr;

		EnsureLanguageFolderAndRussianFile();

		const std::string& dirpath = GetPathffA(Pathff::Language, false);
		std::vector<std::string> allFiles;
		const std::string& ext = ".json";
		get_all_filenames_with_extension(dirpath, ext, allFiles, false);

		for (const std::string& fn : allFiles)
		{
			const std::string& filePath = dirpath + "\\" + fn + ext;
			allLangs.push_back(Lang(fn, filePath));
		}

		bool hasRussian = false;
		for (const auto& lang : allLangs)
		{
			if (lang.GetName() == "Russian")
			{
				hasRussian = true;
				break;
			}
		}
		if (!hasRussian)
			allLangs.push_back(Lang("Russian", dirpath + "\\Russian.json"));

		if (configLangName.empty())
			configLangName = "English";

		SetSelectedLangFromConfig();
		return 0;
	}

	std::string GetSelectedLangTitle()
	{
		return configLangName.empty() ? "English" : configLangName;
	}

	int SetSelectedLangFromConfig()
	{
		std::for_each(allLangs.begin(), allLangs.end(), [](Lang& l) { l.Dictionary().clear(); });

		if (configLangName.empty())
			configLangName = "English";

		if (configLangName == "English")
		{
			selectedLang = nullptr;
			return 0;
		}

		for (auto& lang : allLangs)
		{
			if (lang.GetName() != configLangName)
				continue;

			selectedLang = &lang;
			if (lang.GetName() == "Russian")
				lang.Dictionary() = ParseBuiltinRussian();
			else
				lang.Dictionary().clear();

			std::ifstream stream(lang.GetFilePath(), std::ios::binary);
			try
			{
				if (stream.good())
				{
					Json doc = Json::parse(stream);
					for (auto it = doc.begin(); it != doc.end(); ++it)
					{
						if (it.value().is_string())
							lang.Dictionary()[it.key()] = it.value().get<std::string>();
					}
					addlog(ige::LogType::LOG_INFO, "Loaded language file " + lang.GetFilePath());
				}
				else if (lang.GetName() == "Russian")
				{
					addlog(ige::LogType::LOG_INFO, "Using builtin Russian dictionary");
				}
				else
				{
					addlog(ige::LogType::LOG_ERROR, "Unable to load language file " + lang.GetFilePath());
					return -1;
				}
			}
			catch (...)
			{
				if (lang.GetName() == "Russian")
				{
					lang.Dictionary() = ParseBuiltinRussian();
					addlog(ige::LogType::LOG_INFO, "Russian.json invalid — using builtin dictionary");
				}
				else
				{
					addlog(ige::LogType::LOG_ERROR, "Unable to load language file " + lang.GetFilePath());
					return -1;
				}
			}
			return 0;
		}

		addlog(ige::LogType::LOG_ERROR, "Cannot find selected language. Falling back to English");
		configLangName = "English";
		selectedLang = nullptr;
		return 0;
	}

	int SetSelectedLangFromString(std::string aName)
	{
		configLangName = aName;
		return SetSelectedLangFromConfig();
	}

	void SetSelectedLang(Lang* ptr)
	{
		configLangName.clear();
		selectedLang = ptr;
		if (ptr != nullptr)
			configLangName = selectedLang->GetName();
		SetSelectedLangFromConfig();
	}

	void ResetSelectedLang()
	{
		configLangName = "English";
		selectedLang = nullptr;
	}
}
