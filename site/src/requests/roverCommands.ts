import type { RoverCommand } from "../types/roverCommands";

export async function sendRoverCommand(direction: RoverCommand): Promise<boolean> {
  const config = {
    method: 'POST',
    headers: {
      'Accept': 'application/json',
      'Content-Type': 'application/json'
    },
    body: JSON.stringify({ direction })
  };

  const response = await fetch('/api/rover', config);

  return response.ok;
}