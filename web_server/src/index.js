const express = require("express");
const { SerialPort } = require("serialport");
const app = express();
const port = 3000;

// app.get("/test", (req, res) => {
//     console.log("Hello world!");
//     res.send("Hello World!");
// });

// app.get("/arduino", (req, res) => {
//     // arduino?cmd=""
//     console.log(req.query);
//     res.status(200).send("ok");
// });

// app.get("*", (req, res) => {
//     res.status(404).send("bad");
// });

// app.listen(port, () => {
//     console.log(`Example app listening on port ${port}`)
// });

const serialPort = new SerialPort({ path: "com8", baudRate: 115200 });

serialPort.on("data", (data) => {
    console.log(data.toString());
});