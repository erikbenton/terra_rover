import type TakePhotoResponse from "../types/takePhotoResponse";


export async function takePhoto(photo_name: string): Promise<TakePhotoResponse> {
  const config = {
    method: 'POST',
    headers: {
      'Accept': 'application/json',
      'Content-Type': 'application/json'
    },
    body: JSON.stringify({ photo_name })
  };

  const response = await fetch(`/api/takephoto`, config);

  const photoName = await response.json() as TakePhotoResponse;
  return photoName;
}