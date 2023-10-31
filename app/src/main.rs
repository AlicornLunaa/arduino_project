// Modules
use std::{time::Duration, io::{self, Write}};

use axum::{body::Body, extract::Path, routing::get, Router};
use serialport::{self, SerialPort, SerialPortInfo, SerialPortType};

// Structures
struct ArduinoCommand {
    name: &'static str,
    func: fn() -> bool
}

// Constants
const COMMANDS: [ArduinoCommand; 2] = [
    ArduinoCommand {
        name: "hello_world",
        func: ||{
            println!("hello world!");
            true
        }
    },
    ArduinoCommand {
        name: "button1",
        func: ||{
            println!("Button pressed");
            true
        }
    }
];

// Functions
#[tokio::main]
async fn main() {
    // Scan for ports and try to open them, otherwise start a web server
    let available_ports = serialport::available_ports().expect("No ports found!");

    match connect_to_any_port(&available_ports) {
        Ok(port) => start_port_server(port),
        Err(_) => start_web_server().await,
    }
}

fn connect_to_any_port(available_ports: &Vec<SerialPortInfo>) -> Result<Box<dyn SerialPort>, &str> {
    for port in available_ports {
        if let SerialPortType::UsbPort(usb_info) = &port.port_type {
            let product = usb_info.product.clone().unwrap();

            if product.starts_with("Arduino") {
                match serialport::new(port.port_name.clone(), 115200)
                    .timeout(Duration::from_millis(10))
                    .open()
                {
                    Ok(connected_port) => {
                        return Ok(connected_port);
                    }
                    Err(e) => {
                        println!("{}", e);
                    }
                }
            }
        }
    }

    return Err("no_ports");
}

fn get_function_by_name(name: &String) -> Result<fn() -> bool, &str> {
    for command in COMMANDS {
        if command.name == name {
            return Ok(command.func);
        }
    }

    return Err("bad");
}

fn start_port_server(mut port: Box<dyn SerialPort>) {
    println!("Connected to serial port {} at baud {}", &port.name().unwrap(), &port.baud_rate().unwrap());

    let mut result = "ok";
    let mut serial_buf: Vec<u8> = vec![0; 1000];

    port.write("success".as_bytes()).expect("Port failure");

    loop {
        match port.read(serial_buf.as_mut_slice()) {
            Ok(t) => {
                let cmd = String::from_utf8_lossy(&serial_buf[..t]);
                
                match get_function_by_name(&cmd.to_string()) {
                    Ok(func) => {
                        if !(func)() {
                            result = "bad";
                        }
                    },
                    Err(_) => {
                        result = "bad";
                    }
                }

                port.write(result.as_bytes()).expect("Port failure");
            },
            Err(ref e) if e.kind() == io::ErrorKind::TimedOut => (),
            Err(e) => eprintln!("{:?}", e),
        }
    }
}

async fn start_web_server() {
    println!("Started web server on 0.0.0.0:3000");

    let web_app: Router<(), Body> = Router::new()
        .route("/command/:name", get(command_route));

    axum::Server::bind(&"0.0.0.0:3000".parse().unwrap())
        .serve(web_app.into_make_service())
        .await
        .unwrap();
}

async fn command_route(Path(command_name): Path<String>) -> &'static str {
    match get_function_by_name(&command_name) {
        Ok(func) => {
            if (func)() {
                "ok"
            } else {
                "bad"
            }
        },
        Err(_) => "bad"
    }
}