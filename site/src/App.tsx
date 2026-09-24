import { useState } from 'react'
import './App.css'
import takePhoto from './requests/photos';

function App() {
  const [photoName, setPhotoName] = useState<string | null>(null);
  const [photo, setPhoto] = useState('first');

  const handleTakePhoto = async () => {
    const resp = await takePhoto(photoName ?? "no_name");
    setPhoto(resp.photo_name);
    setPhotoName(null);
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
      {photo !== null &&
        <img className='terra-photo' src={`http://terra.local/api/photos/${photo}.jpg`} />
      }
      <section id="spacer"></section>
    </>
  )
}

export default App