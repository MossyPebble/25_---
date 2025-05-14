// Return the response
client.println("HTTP/1.1 200 OK");
client.println("Content-Type: text/html");
client.println(""); //  do not forget this one
client.println("<!DOCTYPE HTML>");
client.println("<html>");

client.print("Led pin is now: ");

if(value == HIGH) {
    client.print("On");  
} else {
    client.print("Off");
}
client.println("<br><br>");
client.println("Click <a href=\"/LED=ON\">here</a> turn the LED on pin 5 ON<br>");
client.println("Click <a href=\"/LED=OFF\">here</a> turn the LED on pin 5 OFF<br>");

client.println("Click <a href=\"/Motor=ON\">here</a> turn the Motor ON<br>");
client.println("Click <a href=\"/Motor=OFF\">here</a> turn the Motor OFF<br>");

client.println("Click <a href=\"/Speed=LOW\">here</a> turn the Motor Speed LOW<br>");
client.println("Click <a href=\"/Speed=HIGH\">here</a> turn the Motor Speed HIGH<br>");

client.println("</html>");

delay(1);
Serial.println("Client disconnected");
Serial.println("");