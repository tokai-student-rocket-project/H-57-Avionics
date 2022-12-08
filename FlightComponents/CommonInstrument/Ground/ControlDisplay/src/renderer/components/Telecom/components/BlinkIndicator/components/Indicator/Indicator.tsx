type Props = {
  isActive: boolean;
};

const Indicator = (props: Props) => {
  const { isActive } = props;

  return (
    <div
      style={{
        backgroundColor: isActive ? '#46c46d' : '#72767d',
        width: '12px',
        margin: 'auto 16px auto 8px',
        height: '12px',
        borderRadius: '50%',
      }}
    />
  );
};

export default Indicator;
