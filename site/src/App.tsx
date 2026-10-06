import { useState } from 'react'
import './App.css'
import { takePhoto } from './requests/photos';
import type { RoverCommand } from './types/roverCommands';
import { sendRoverCommand } from './requests/roverCommands';

function App() {
  const [photoName, setPhotoName] = useState<string | null>(null);
  const [photo, setPhoto] = useState<string | null>(null);

  const handleTakePhoto = async () => {
    const parsedName = photoName?.split(' ').join('_');
    const resp = await takePhoto(parsedName ?? "no_name");
    setPhoto(resp.photo_name);
    setPhotoName(null);
  }

  const sendCommand = async (direction: RoverCommand) => {
    const resp = await sendRoverCommand(direction);
    if (!resp) {
      console.log(`Error sending direction: ${direction}`);
    }
  }

  return (
    <>
      <section id="center">
        <div>
          <h1>Take a picture!</h1>
        </div>
        <input type='text' value={photoName ?? ''} onChange={(e) => setPhotoName(e.target.value)} />
        <button
          type="button"
          className="camera-btn"
          onClick={handleTakePhoto}
        >
          snap
        </button>
      </section>

      <section className='rover-controls'>
        <div className='control-box'>
          <div className='row centered'>
            <button className='direction-btn' id='forward-button' onClick={() => sendCommand('forward')}>
              Forward
            </button>
          </div>
          <div className='row evenly-split'>
            <button className='direction-btn' id='left-button' onClick={() => sendCommand('left')}>
              Left
            </button>
            <button className='direction-btn' id='right-button' onClick={() => sendCommand('right')}>
              Right
            </button>
          </div>
          <div className='row centered'>
            <button className='direction-btn' id='back-button' onClick={() => sendCommand('back')}>
              Back
            </button>
          </div>
        </div>
      </section>

      {photo !== null &&
        <img className='terra-photo' src={`http://terra.local/api/photos/${photo}.jpg`} />
      }
      <section id="spacer"></section>
    </>
  )
}

export default App