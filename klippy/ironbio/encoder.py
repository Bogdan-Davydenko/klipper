from extras.gcode_macro import GCodeMacro

# TODO:
# Это файл модуля инкриментального энкодера с A/B сигналами. На стороне хоста нужно реализовать:
# - Команду запроса позиции энкодера CHECK_ENCODER
# - Команду включения постоянных ответов раз в некоторое время DEBUG_ENCODER
# - Регистрацию ответа о позиции от платы
# - Получение пинов и mcu в printer.cfg
# - Решить вопрос с регистрацией нескольких энкодеров.

class PrinterEncoder:
    def __init__(self, config):
        self.printer = config.get_printer()
        self.gcode = self.printer.lookup_object('gcode')
        self.mcu = self.printer.lookup_object('mcu') # Вероятно нужно будет изменить метод получения mcu, если mcu будет несколько.
        self.oid = self.mcu.create_oid()

        # Регистрация команд
        self.config_cmd = self.mcu.lookup_command(
            "config_encoder oid=%c")
        self.config_cmd = self.mcu.lookup_command(
            "check_encoder oid=%c")

        self.name = config.get_name().split()[1]
        
        self.gcode.register_mux_command("ENC_TEST", "ENCODER", self.name,
                                   self.cmd_ENC_TEST,
                                   desc=self.cmd_ENC_TEST_help)
        # Отправка конфигурации на mcu
        self.config_cmd.send([self.oid])
        # Регистрация ответов от mcu
        self.mcu.register_response(
            self._handle_encoder_response, 
            "encoder_" + str(self.oid) +":"
        )

    def _handle_encoder_response(self, params):
        self.gcode.respond_raw("Encoder " + self.name + " position is: " + params.get('pos', 0))

    cmd_ENC_TEST_help = "Encoder test command"
    def cmd_ENC_TEST(self, gcmd):
        cmd = self.mcu.lookup_command("helloworld") # Создание команды
        cmd.send() # Отправка команды
        gcmd.respond_info(self.name) # Ответ об отправлении команды

def load_config_prefix(config):
    return PrinterEncoder(config)
