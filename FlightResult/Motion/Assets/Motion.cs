using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class Motion : MonoBehaviour
{
    public FlightData[] flightData;

    private int offset = 0;

    [SerializeField]
    public TextMeshProUGUI flightDataDisplay;

    // Start is called before the first frame update
    void Start()
    {
        TextAsset textAsset = new TextAsset();
        textAsset = Resources.Load("Flight", typeof(TextAsset)) as TextAsset;
        flightData = CSVSerializer.Deserialize<FlightData>(textAsset.text);
    }

    // Update is called once per frame
    void Update()
    {
        if (flightData.Length <= offset)
        {
            UnityEditor.EditorApplication.isPlaying = false;
            return;
        }

        float flightTime = flightData[offset].flightTime;
        if (Time.time < flightTime) return;

        float yaw = flightData[offset].yaw;
        float pitch = flightData[offset].pitch;
        float roll = flightData[offset].roll;

        flightDataDisplay.text = $"X + {flightTime.ToString("n2")} sec\n\nAlt [m] :\t {flightData[offset].altitude}\nYaw [deg] :\t {yaw}\nPitch [deg] :\t {pitch}\nRoll [deg] :\t {roll}\n\nShiranui :\t {((flightData[offset].stateShiranui3 == 0) ? ' ' : '¡')}\nBuzzer :\t {((flightData[offset].stateBuzzer == 0) ? ' ' : '¡')}";

        transform.rotation = Quaternion.Euler(pitch, yaw + 90f, roll);

        offset++;
    }
}
