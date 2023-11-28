// Modules
use std::{time::Duration, io::{self, Write}};

use axum::{body::Body, extract::Path, routing::get, Router};
use serialport::{self, SerialPort, SerialPortInfo, SerialPortType};
use rsautogui::{mouse, keyboard, screen};

// Functions
#[tokio::main]
async fn main() {
    // Scan for ports and try to open them, otherwise start a web server
    let available_ports = serialport::available_ports().expect("No ports found!");

    match connect_to_any_port(&available_ports) {
        Ok(port) => start_port_server(port).await,
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

async fn start_port_server(mut port: Box<dyn SerialPort>) {
    println!("Connected to serial port {} at baud {}", &port.name().unwrap(), &port.baud_rate().unwrap());

    let mut result = "ok";
    let mut serial_buf: Vec<u8> = vec![0; 1000];

    port.write("success".as_bytes()).expect("Port failure");

    loop {
        match port.read(serial_buf.as_mut_slice()) {
            Ok(t) => {
                let cmd = String::from_utf8_lossy(&serial_buf[..t]);
                
                if !run_by_name(&cmd.to_string()).await {
                    result = "bad";
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
    let res = run_by_name(&command_name).await;

    if res {
        return "ok";
    }

    "bad"
}

async fn run_by_name(name: &String) -> bool {
    match name.as_str() {
        "button1" => button1().await,
        _ => false
    }
}

async fn button1() -> bool {
    println!("Button 1 pressed!");
    tweet("can we please get a good grade");
    return true;
}

fn tweet(content: &str) {
    let (width, height) = screen::size();
    let first_x = (873.0 / 1920.0 * (width as f32)) as u16;
    let first_y = (164.0 / 1080.0 * (height as f32)) as u16;
    let second_x = (1134.0 / 1920.0 * (width as f32)) as u16;
    let second_y = (254.0 / 1080.0 * (height as f32)) as u16;

    let rand_num = rand::random::<u16>();
    let rand_str = format!("{}", rand_num);
    let msg = String::from(content) + " " + rand_str.as_str();

    mouse::move_to(first_x, first_y);
    mouse::click(mouse::Button::Left);
    keyboard::typewrite(msg.as_str());
    mouse::move_to(second_x, second_y);
    mouse::click(mouse::Button::Left);
}