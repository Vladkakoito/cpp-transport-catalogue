Транспортный справочник

Упражнение с наследованием классов, созданием SVG-изображений, работой с JSON-файлами. Стандарт С++ 17.


Строение проекта

tr_cat::aggregations::TransportCatalogue класс, с информацией о маршрутах и остановках. Создается без аргументов.
tr_cat::interface::RequestInterface класс для работы с каталогом, чтения и вывода данных. Аргументы — ссылка на класс TransportCatalogue, поток вывода и ввода (по умолчанию std::cin, std::cout).

Список методов объекта RequestInterface:
	ReadDocument()
	ParseDocument()
	AddStops()      //
	AddDistances()  //для корректного заполнения базы нужно соблюдать порядок
	AddBuses()      //
	GetAnswers()
	PrintAnswers()

	bool TestingFilesOutput("file1", "file2") //сравнение двух файлов выходного формата
	tr_cat::render::RenderSettings GetRenderSettings() //настройки рендера


На данный момент чтение базы и запросов только из JSON.
Запись в каталог базы и вывод ответов на запросы в формате JSON автоматизирован в функции tr_cat::interface::Process()

Программа фрагментирована для тестирования отдельных модулей. 
Для теста скорости  запустить tr_cat::tests::TestCatalogSpeed(file_input, file_output)
Для теста ответа запустить tr_cat::tests::TestOutput(file_input, file_output, file_example)
Для теста скорости рендера в SVG документ запустить TestRenderSpeed(file_input, file_output)

Формат JSON для входного потока
{
	"base_requests": [
		{
			"Type": "Stop",
			"name": "stop_name",
			"latitude": double
			"longitude": double
			"road_distances": {
				"stop_name": int //расстояние до др остановок в метрах
				…
			}
		},
		{
			"Type": "Bus",
			"name": "bus_name",
			"stops": [
				"stop1",
				"stop2",
				…
			],
			"is_roundtrip": true/false //кольцевой/некольцевой маршрут
		},
		…
	],
	"stat_request": [ 
		{
			"id": int,
			"type": "Stop"/"Bus",
			"name": "name"
		},
		{	"id": int,
			"type": "map" //вывод карты в формате SVG
		},
		…
	],
	"render_settings": {
		"width": 1200.0, 	//ширина и высота изображения в пикселях. 
		"height": 1200.0, //Вещественное число в диапазоне от 0 до 100000.
		"padding": 50.0, //отступ краёв карты от границ SVG-документа
		"line_width": 14.0, //толщина линий, которыми рисуются автобусные маршруты
		"stop_radius": 5.0, //радиус окружностей, которыми обозначаются остановки.
		"bus_label_font_size": 20,//размер текста, которым написаны названия автобусных маршрутов 
		"bus_label_offset": [7.0, 15.0], //смещение надписи с названием маршрута относительно координат конечной остановки на карте
		"stop_label_font_size": 20, //размер текста, которым отображаются названия остановок
		"stop_label_offset": [7.0, -3.0], //смещение названия остановки относительно её координат на карте
		"underlayer_color": [255, 255, 255, 0.85], //цвет подложки под названиями остановок и маршрутов
		"underlayer_width": 3.0, //толщина подложки под названиями остановок и маршрутов
		"color_palette": [  //палитра цветов. Цвет может быть задан в 
			"green",      //текстовом формате, либо в формате 
			[255, 160, 0],//rgb, либо в формате rgba
			"red" 
		] 
	}
}

Для вывода отдельного SVG-документа с картой нужно создать объект 
tr_cat::render::MapRenderer  с аргументами: tr_cat::aggregations::TransportCatalogue&,
tr_cat::render::RenderSettings //возвращает метод GetRenderSettings класса 						 //tr_cat_interface::RequestInterface
output   //выходной поток, по умолчанию std::cout

И вывести карту методом Render, 
либо воспользоваться TestRenderSpeed(file_input, file_output)

Образец JSON документов в папке test_files.


Планируемая доработка: 
Добавление возможности работы с фалами XML
Добавление обработки некорректных документов
